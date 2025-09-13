import psutil
import time
from threading import Thread
from flask_socketio import emit

class SystemMonitor:
    def __init__(self, socketio):
        self.socketio = socketio
        self.monitoring_threads = {}
    
    def start(self, system_id):
        if system_id in self.monitoring_threads:
            return
        
        def monitor():
            prev_net_in = psutil.net_io_counters().bytes_recv
            prev_net_out = psutil.net_io_counters().bytes_sent
            
            while system_id in self.monitoring_threads:
                # Get current stats
                cpu = psutil.cpu_percent()
                mem = psutil.virtual_memory().percent
                disk = psutil.disk_usage('/').percent
                
                # Calculate network speed
                net = psutil.net_io_counters()
                net_in = (net.bytes_recv - prev_net_in) / 1024
                net_out = (net.bytes_sent - prev_net_out) / 1024
                prev_net_in, prev_net_out = net.bytes_recv, net.bytes_sent
                
                # Emit stats
                self.socketio.emit('system_stats', {
                    'system_id': system_id,
                    'stats': {
                        'cpu': cpu,
                        'memory': mem,
                        'disk': disk,
                        'network_in': net_in,
                        'network_out': net_out
                    }
                })
                
                time.sleep(1)
        
        self.monitoring_threads[system_id] = True
        Thread(target=monitor).start()
    
    def stop(self, system_id):
        if system_id in self.monitoring_threads:
            del self.monitoring_threads[system_id]