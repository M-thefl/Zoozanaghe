import os
import psutil
import platform
from datetime import datetime

class SystemManager:
    def __init__(self):
        self.systems = {}
    
    def register_system(self, user_id, name=None):
        system_id = f"{platform.node()}-{os.getpid()}"
        system = {
            'id': system_id,
            'name': name or platform.node(),
            'ip': self._get_ip(),
            'os': f"{platform.system()} {platform.release()}",
            'cpu_cores': os.cpu_count(),
            'ram': round(psutil.virtual_memory().total / (1024**3), 2),
            'last_seen': datetime.now(),
            'user_id': user_id,
            'is_active': True
        }
        self.systems[system_id] = system
        return system
    
    def get_user_systems(self, user_id):
        return [s for s in self.systems.values() if s['user_id'] == user_id]
    
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