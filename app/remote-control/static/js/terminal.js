document.addEventListener('DOMContentLoaded', function() {
    const socket = io();
    const terminalInput = document.getElementById('terminal-command');
    const terminalOutput = document.getElementById('terminal-output');
    const systemId = "{{ system_id }}";
    
    terminalInput.focus();
    
    terminalInput.addEventListener('keydown', function(e) {
        if (e.key === 'Enter') {
            const command = this.value;
            this.value = '';
            
            // Display command
            addTerminalLine(`$ ${command}`, 'command');
            
            // Send to server
            socket.emit('terminal_command', {
                system_id: systemId,
                command: command
            });
        }
    });
    
    socket.on('terminal_output', function(data) {
        if (data.system_id === systemId) {
            addTerminalLine(data.output, data.is_error ? 'error' : 'output');
        }
    });
    
    function addTerminalLine(text, type) {
        const line = document.createElement('div');
        line.className = `terminal-line ${type}`;
        line.textContent = text;
        terminalOutput.appendChild(line);
        terminalOutput.scrollTop = terminalOutput.scrollHeight;
    }
});