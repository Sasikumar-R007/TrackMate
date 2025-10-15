let userInfo = null;
let studentInfo = null;
let busDetails = null;

// Initialize
async function init() {
    await loadUserInfo();
    await loadBusDetails();
    await loadStudentInfo();
    await loadTodayAttendance();
    setDefaultDate();
}

// Load user info
async function loadUserInfo() {
    try {
        const response = await fetch('/api/user-info');
        if (response.ok) {
            userInfo = await response.json();
            document.getElementById('userName').textContent = userInfo.name;
        } else {
            window.location.href = '/';
        }
    } catch (error) {
        window.location.href = '/';
    }
}

// Show notification
function showNotification(message, type = 'success') {
    const notification = document.getElementById('notification');
    notification.textContent = message;
    notification.className = `notification ${type} show`;
    
    setTimeout(() => {
        notification.classList.remove('show');
    }, 3000);
}

// Show section
function showSection(section) {
    document.querySelectorAll('.section').forEach(s => s.classList.remove('active'));
    document.querySelectorAll('.menu-item').forEach(m => m.classList.remove('active'));
    
    document.getElementById(`${section}Section`).classList.add('active');
    event.target.closest('.menu-item').classList.add('active');
}

// Logout
async function logout() {
    await fetch('/api/logout', { method: 'POST' });
    window.location.href = '/';
}

// Load bus details
async function loadBusDetails() {
    try {
        const response = await fetch(`/api/bus-details/${userInfo.bus_number}`);
        if (response.ok) {
            busDetails = await response.json();
            displayBusDetails();
        }
    } catch (error) {
        showNotification('Error loading bus details', 'error');
    }
}

// Display bus details
function displayBusDetails() {
    if (busDetails) {
        document.getElementById('busNumber').textContent = busDetails.bus_number || '-';
        document.getElementById('busRoute').textContent = busDetails.bus_route || '-';
        document.getElementById('driverName').textContent = busDetails.driver_name || '-';
        document.getElementById('driverMobile').textContent = busDetails.driver_mobile || '-';
        document.getElementById('teacherName').textContent = busDetails.teacher_name || 'Not assigned';
        document.getElementById('teacherMobile').textContent = busDetails.teacher_mobile || '-';
    }
}

// Load student info
async function loadStudentInfo() {
    try {
        const response = await fetch('/api/students');
        if (response.ok) {
            const students = await response.json();
            if (students.length > 0) {
                studentInfo = students[0];
                displayStudentInfo();
            }
        }
    } catch (error) {
        showNotification('Error loading student info', 'error');
    }
}

// Display student info
function displayStudentInfo() {
    const container = document.getElementById('studentInfo');
    if (studentInfo) {
        container.innerHTML = `
            <div class="student-card" style="max-width: 400px;">
                <h4>👦 ${studentInfo.student_name}</h4>
                <p>🎓 Roll Number: ${studentInfo.roll_number}</p>
                <p>📚 Class: ${studentInfo.class}</p>
                <p>🚌 Bus: ${studentInfo.bus_number}</p>
                <p>🛣️ Route: ${studentInfo.bus_route}</p>
            </div>
        `;
    }
}

// Set default date
function setDefaultDate() {
    const today = new Date().toISOString().split('T')[0];
    document.getElementById('dateFilter').value = today;
    document.getElementById('dateFilter').max = today;
}

// Load today's attendance
async function loadTodayAttendance() {
    if (!studentInfo) return;
    
    const container = document.getElementById('todayRecord');
    const today = new Date().toISOString().split('T')[0];
    
    try {
        const response = await fetch(`/api/attendance?student_id=${studentInfo.id}&date=${today}`);
        if (response.ok) {
            const records = await response.json();
            
            if (records.length === 0) {
                container.innerHTML = '<p class="empty-state">No attendance record for today</p>';
                return;
            }
            
            const record = records[0];
            container.innerHTML = `
                <div class="attendance-detail-card">
                    <div class="detail-row">
                        <span class="detail-label">📅 Date:</span>
                        <span class="detail-value">${formatDate(record.date)}</span>
                    </div>
                    <div class="detail-row">
                        <span class="detail-label">⏰ Entry Time:</span>
                        <span class="detail-value ${record.entry_time ? 'present' : 'absent'}">
                            ${record.entry_time || 'Not marked'}
                        </span>
                    </div>
                    <div class="detail-row">
                        <span class="detail-label">⏰ Exit Time:</span>
                        <span class="detail-value ${record.exit_time ? 'present' : 'absent'}">
                            ${record.exit_time || 'Not marked'}
                        </span>
                    </div>
                    <div class="detail-row">
                        <span class="detail-label">🚌 Bus:</span>
                        <span class="detail-value">${record.bus_number}</span>
                    </div>
                </div>
            `;
        }
    } catch (error) {
        container.innerHTML = '<p class="empty-state">Error loading attendance</p>';
    }
}

// Load attendance by date
async function loadAttendanceByDate() {
    if (!studentInfo) return;
    
    const container = document.getElementById('attendanceHistory');
    const date = document.getElementById('dateFilter').value;
    
    try {
        const response = await fetch(`/api/attendance?student_id=${studentInfo.id}&date=${date}`);
        if (response.ok) {
            const records = await response.json();
            
            if (records.length === 0) {
                container.innerHTML = '<p class="empty-state">No attendance record for this date</p>';
                return;
            }
            
            const record = records[0];
            container.innerHTML = `
                <div class="attendance-detail-card">
                    <div class="detail-row">
                        <span class="detail-label">📅 Date:</span>
                        <span class="detail-value">${formatDate(record.date)}</span>
                    </div>
                    <div class="detail-row">
                        <span class="detail-label">⏰ Entry Time:</span>
                        <span class="detail-value ${record.entry_time ? 'present' : 'absent'}">
                            ${record.entry_time || 'Not marked'}
                        </span>
                    </div>
                    <div class="detail-row">
                        <span class="detail-label">⏰ Exit Time:</span>
                        <span class="detail-value ${record.exit_time ? 'present' : 'absent'}">
                            ${record.exit_time || 'Not marked'}
                        </span>
                    </div>
                    <div class="detail-row">
                        <span class="detail-label">🚌 Bus:</span>
                        <span class="detail-value">${record.bus_number}</span>
                    </div>
                </div>
            `;
        }
    } catch (error) {
        container.innerHTML = '<p class="empty-state">Error loading attendance</p>';
    }
}

// Format date
function formatDate(dateStr) {
    const date = new Date(dateStr);
    return date.toLocaleDateString('en-US', { 
        weekday: 'long', 
        year: 'numeric', 
        month: 'long', 
        day: 'numeric' 
    });
}

// Add CSS for attendance details
const style = document.createElement('style');
style.textContent = `
    .attendance-detail-card {
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        color: white;
        padding: 25px;
        border-radius: 12px;
    }
    
    .detail-row {
        display: flex;
        justify-content: space-between;
        padding: 12px 0;
        border-bottom: 1px solid rgba(255,255,255,0.2);
    }
    
    .detail-row:last-child {
        border-bottom: none;
    }
    
    .detail-label {
        font-weight: 600;
        opacity: 0.9;
    }
    
    .detail-value {
        font-weight: 700;
    }
    
    .detail-value.present {
        color: #43e97b;
    }
    
    .detail-value.absent {
        color: #f5576c;
    }
    
    .student-info .student-card {
        background: linear-gradient(135deg, #43e97b 0%, #38f9d7 100%);
    }
`;
document.head.appendChild(style);

// Initialize on load
window.addEventListener('DOMContentLoaded', init);
