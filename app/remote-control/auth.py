from functools import wraps
from flask import session, redirect, url_for

USERS = {
    'admin': {'password': 'admin123', 'is_admin': True},
    'user': {'password': 'user123', 'is_admin': False}
}

def authenticate(username, password):
    if username in USERS and USERS[username]['password'] == password:
        session['user_id'] = username
        session['is_admin'] = USERS[username]['is_admin']
        return True
    return False

def requires_auth(f):
    @wraps(f)
    def decorated(*args, **kwargs):
        if 'user_id' not in session:
            return redirect(url_for('login'))
        return f(*args, **kwargs)
    return decorated