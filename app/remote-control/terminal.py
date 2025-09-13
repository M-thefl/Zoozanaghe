import subprocess
from threading import Thread
from flask_socketio import emit

class Terminal:
    def __init__(self, socketio):
        self.socketio = socketio
        self.sessions = {}
    
    def execute(self, system_id, command):
        def run_command():
            process = subprocess.Popen(
                command,
                shell=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            
            for line in process.stdout:
                self.socketio.emit('terminal_output', {
                    'system_id': system_id,
                    'output': line
                })
            
            for line in process.stderr:
                self.socketio.emit('terminal_output', {
                    'system_id': system_id,
                    'output': line,
                    'is_error': True
                })
        
        Thread(target=run_command).start()