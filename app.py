from flask import Flask, request, jsonify, session, send_from_directory
from flask_cors import CORS
from werkzeug.security import generate_password_hash, check_password_hash
import sqlite3
import os
from datetime import datetime
import secrets

app = Flask(__name__, static_folder='static', static_url_path='')
app.secret_key = secrets.token_hex(32)
CORS(app)

DATABASE = 'trackmate.db'

# Bus routes mapping
BUS_ROUTES = {
    'Bus-1': 'Route A - Main Street → School',
    'Bus-2': 'Route B - Park Avenue → School',
    'Bus-3': 'Route C - Lake Road → School',
    'Bus-4': 'Route D - Hill Street → School'
}

def get_db():
    conn = sqlite3.connect(DATABASE)
    conn.row_factory = sqlite3.Row
    return conn

def init_db():
    conn = get_db()
    c = conn.cursor()
    
    # Users table (teachers and parents)
    c.execute('''CREATE TABLE IF NOT EXISTS users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        user_type TEXT NOT NULL,
        name TEXT NOT NULL,
        mobile TEXT,
        password TEXT NOT NULL,
        class TEXT,
        students_count INTEGER,
        bus_number TEXT,
        bus_route TEXT,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )''')
    
    # Students table
    c.execute('''CREATE TABLE IF NOT EXISTS students (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        student_name TEXT NOT NULL,
        roll_number TEXT UNIQUE NOT NULL,
        class TEXT NOT NULL,
        parent_id INTEGER,
        bus_number TEXT,
        bus_route TEXT,
        fingerprint_id INTEGER,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY (parent_id) REFERENCES users(id)
    )''')
    
    # Bus details table
    c.execute('''CREATE TABLE IF NOT EXISTS bus_details (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        bus_number TEXT UNIQUE NOT NULL,
        bus_route TEXT NOT NULL,
        driver_name TEXT,
        driver_mobile TEXT,
        teacher_id INTEGER,
        capacity INTEGER,
        FOREIGN KEY (teacher_id) REFERENCES users(id)
    )''')
    
    # Attendance table
    c.execute('''CREATE TABLE IF NOT EXISTS attendance (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        student_id INTEGER,
        entry_time TIMESTAMP,
        exit_time TIMESTAMP,
        date DATE,
        bus_number TEXT,
        FOREIGN KEY (student_id) REFERENCES students(id)
    )''')
    
    # Insert default bus details
    buses = [
        ('Bus-1', 'Route A - Main Street → School', 'Rajesh Kumar', '9876543210', None, 40),
        ('Bus-2', 'Route B - Park Avenue → School', 'Suresh Babu', '9876543211', None, 35),
        ('Bus-3', 'Route C - Lake Road → School', 'Ramesh Reddy', '9876543212', None, 45),
        ('Bus-4', 'Route D - Hill Street → School', 'Vijay Singh', '9876543213', None, 38)
    ]
    
    c.execute('SELECT COUNT(*) FROM bus_details')
    if c.fetchone()[0] == 0:
        c.executemany('''INSERT INTO bus_details (bus_number, bus_route, driver_name, driver_mobile, teacher_id, capacity)
                        VALUES (?, ?, ?, ?, ?, ?)''', buses)
    
    conn.commit()
    conn.close()

# Initialize database
init_db()

@app.route('/')
def index():
    return send_from_directory('static', 'login.html')

@app.route('/api/register', methods=['POST'])
def register():
    data = request.json
    user_type = data.get('user_type')
    
    conn = get_db()
    c = conn.cursor()
    
    try:
        if user_type == 'teacher':
            hashed_pw = generate_password_hash(data.get('password'))
            c.execute('''INSERT INTO users (user_type, name, mobile, password, class, students_count, bus_number, bus_route)
                        VALUES (?, ?, ?, ?, ?, ?, ?, ?)''',
                     (user_type, data.get('name'), data.get('mobile'), hashed_pw, 
                      data.get('class'), data.get('students_count'), 
                      data.get('bus_number'), data.get('bus_route')))
            
            # Update bus details with teacher
            c.execute('UPDATE bus_details SET teacher_id = ? WHERE bus_number = ?',
                     (c.lastrowid, data.get('bus_number')))
        
        elif user_type == 'parent':
            hashed_pw = generate_password_hash(data.get('password'))
            c.execute('''INSERT INTO users (user_type, name, mobile, password, class, bus_number, bus_route)
                        VALUES (?, ?, ?, ?, ?, ?, ?)''',
                     (user_type, data.get('name'), data.get('mobile'), hashed_pw,
                      data.get('class'), data.get('bus_number'), data.get('bus_route')))
            
            parent_id = c.lastrowid
            
            # Create student entry
            c.execute('''INSERT INTO students (student_name, roll_number, class, parent_id, bus_number, bus_route)
                        VALUES (?, ?, ?, ?, ?, ?)''',
                     (data.get('student_name'), data.get('roll_number'), data.get('class'),
                      parent_id, data.get('bus_number'), data.get('bus_route')))
        
        conn.commit()
        return jsonify({'success': True, 'message': 'Registration successful'})
    except sqlite3.IntegrityError as e:
        return jsonify({'success': False, 'message': 'User already exists or invalid data'}), 400
    finally:
        conn.close()

@app.route('/api/login', methods=['POST'])
def login():
    data = request.json
    user_type = data.get('user_type')
    password = data.get('password')
    
    conn = get_db()
    c = conn.cursor()
    
    if user_type == 'teacher':
        # Teachers login with name
        name = data.get('name')
        c.execute('SELECT * FROM users WHERE name = ? AND user_type = ?', (name, user_type))
        user = c.fetchone()
    else:
        # Parents login with student roll number
        roll_number = data.get('roll_number')
        # Find parent through student roll number
        c.execute('''SELECT u.* FROM users u 
                     JOIN students s ON u.id = s.parent_id 
                     WHERE s.roll_number = ? AND u.user_type = ?''', (roll_number, user_type))
        user = c.fetchone()
    
    conn.close()
    
    if user and check_password_hash(user['password'], password):
        session['user_id'] = user['id']
        session['user_type'] = user['user_type']
        session['user_name'] = user['name']
        
        return jsonify({
            'success': True,
            'user_type': user['user_type'],
            'name': user['name']
        })
    
    return jsonify({'success': False, 'message': 'Invalid credentials'}), 401

@app.route('/api/logout', methods=['POST'])
def logout():
    session.clear()
    return jsonify({'success': True})

@app.route('/api/bus-routes', methods=['GET'])
def get_bus_routes():
    return jsonify(BUS_ROUTES)

@app.route('/api/bus-details/<bus_number>', methods=['GET'])
def get_bus_details(bus_number):
    conn = get_db()
    c = conn.cursor()
    
    c.execute('''SELECT bd.*, u.name as teacher_name, u.mobile as teacher_mobile
                 FROM bus_details bd
                 LEFT JOIN users u ON bd.teacher_id = u.id
                 WHERE bd.bus_number = ?''', (bus_number,))
    bus = c.fetchone()
    conn.close()
    
    if bus:
        return jsonify(dict(bus))
    return jsonify({'error': 'Bus not found'}), 404

@app.route('/api/students', methods=['GET'])
def get_students():
    conn = get_db()
    c = conn.cursor()
    
    if session.get('user_type') == 'teacher':
        bus_number = request.args.get('bus_number')
        c.execute('SELECT * FROM students WHERE bus_number = ?', (bus_number,))
    else:
        c.execute('SELECT * FROM students WHERE parent_id = ?', (session.get('user_id'),))
    
    students = [dict(row) for row in c.fetchall()]
    conn.close()
    return jsonify(students)

@app.route('/api/attendance', methods=['GET', 'POST'])
def attendance():
    conn = get_db()
    c = conn.cursor()
    
    if request.method == 'POST':
        data = request.json
        c.execute('''INSERT INTO attendance (student_id, entry_time, exit_time, date, bus_number)
                    VALUES (?, ?, ?, ?, ?)''',
                 (data.get('student_id'), data.get('entry_time'), data.get('exit_time'),
                  data.get('date'), data.get('bus_number')))
        conn.commit()
        conn.close()
        return jsonify({'success': True})
    
    # GET attendance
    student_id = request.args.get('student_id')
    date = request.args.get('date', datetime.now().strftime('%Y-%m-%d'))
    
    c.execute('''SELECT a.*, s.student_name FROM attendance a
                 JOIN students s ON a.student_id = s.id
                 WHERE a.student_id = ? AND a.date = ?''',
             (student_id, date))
    
    records = [dict(row) for row in c.fetchall()]
    conn.close()
    return jsonify(records)

@app.route('/api/user-info', methods=['GET'])
def get_user_info():
    if 'user_id' not in session:
        return jsonify({'error': 'Not logged in'}), 401
    
    conn = get_db()
    c = conn.cursor()
    c.execute('SELECT id, user_type, name, mobile, class, bus_number, bus_route FROM users WHERE id = ?', 
              (session['user_id'],))
    user = c.fetchone()
    conn.close()
    
    if user:
        return jsonify(dict(user))
    return jsonify({'error': 'User not found'}), 404

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
