let userInfo = null;
let students = [];

// Initialize
async function init() {
    await loadUserInfo();
    await loadStudents();
    await loadAttendance();
    setCurrentTime();
    populateTeacherDetails();
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

// Populate teacher details
function populateTeacherDetails() {
    if (userInfo) {
        document.getElementById('teacherNameField').value = userInfo.name || '';
        document.getElementById('teacherMobileField').value = userInfo.mobile || '';
        document.getElementById('teacherClassField').value = userInfo.class || '';
        document.getElementById('teacherBusField').value = userInfo.bus_number || '';
        document.getElementById('teacherRouteField').value = userInfo.bus_route || '';
    }
}

// Update teacher details
async function updateTeacherDetails() {
    const name = document.getElementById('teacherNameField').value;
    const mobile = document.getElementById('teacherMobileField').value;
    
    if (!name || !mobile) {
        showNotification('Please fill all fields!', 'error');
        return;
    }
    
    try {
        const response = await fetch('/api/update-teacher', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name, mobile })
        });
        
        const data = await response.json();
        if (data.success) {
            showNotification('Details updated successfully!', 'success');
            userInfo.name = name;
            userInfo.mobile = mobile;
            document.getElementById('userName').textContent = name;
        } else {
            showNotification(data.message || 'Update failed!', 'error');
        }
    } catch (error) {
        showNotification('Connection error!', 'error');
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

// Load students
async function loadStudents() {
    try {
        const response = await fetch(`/api/students?bus_number=${userInfo.bus_number}`);
        students = await response.json();
        
        displayStudents(students);
        populateVerifyDropdown(students);
    } catch (error) {
        showNotification('Error loading students', 'error');
    }
}

// Display students
function displayStudents(studentsList) {
    const container = document.getElementById('studentsList');
    
    if (studentsList.length === 0) {
        container.innerHTML = '<p class="empty-state">No students enrolled yet</p>';
        return;
    }
    
    container.innerHTML = studentsList.map(student => `
        <div class="student-card">
            <h4>👦 ${student.student_name}</h4>
            <p>🎓 Roll: ${student.roll_number}</p>
            <p>📚 Class: ${student.class}</p>
            <p>🚌 ${student.bus_number}</p>
        </div>
    `).join('');
}

// Filter students
function filterStudents() {
    const search = document.getElementById('searchStudent').value.toLowerCase();
    const filtered = students.filter(s => 
        s.student_name.toLowerCase().includes(search) ||
        s.roll_number.toLowerCase().includes(search) ||
        s.class.toLowerCase().includes(search)
    );
    displayStudents(filtered);
}

// Populate verify dropdown
function populateVerifyDropdown(studentsList) {
    const select = document.getElementById('verifyStudent');
    select.innerHTML = '<option value="">Select Student</option>' +
        studentsList.map(s => 
            `<option value="${s.id}">${s.student_name} - ${s.roll_number}</option>`
        ).join('');
}

// Set current time
function setCurrentTime() {
    const now = new Date();
    const time = now.toTimeString().slice(0, 5);
    document.getElementById('entryTime').value = time;
}

// Enroll student
async function enrollStudent() {
    const name = document.getElementById('enrollName').value;
    const roll = document.getElementById('enrollRoll').value;
    const studentClass = document.getElementById('enrollClass').value;
    const fingerId = document.getElementById('enrollFingerId').value;
    
    if (!name || !roll || !studentClass || !fingerId) {
        showNotification('Please fill all fields!', 'error');
        return;
    }
    
    try {
        const response = await fetch('/api/register', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                user_type: 'parent',
                name: 'Parent of ' + name,
                mobile: '0000000000',
                password: 'default123',
                student_name: name,
                roll_number: roll,
                class: studentClass,
                bus_number: userInfo.bus_number,
                bus_route: userInfo.bus_route
            })
        });
        
        if (response.ok) {
            showNotification('Student enrolled successfully!', 'success');
            document.getElementById('enrollName').value = '';
            document.getElementById('enrollRoll').value = '';
            document.getElementById('enrollClass').value = '';
            document.getElementById('enrollFingerId').value = '';
            await loadStudents();
        } else {
            showNotification('Enrollment failed!', 'error');
        }
    } catch (error) {
        showNotification('Connection error!', 'error');
    }
}

// Mark attendance
async function markAttendance() {
    const studentId = document.getElementById('verifyStudent').value;
    const entryTime = document.getElementById('entryTime').value;
    const exitTime = document.getElementById('exitTime').value;
    
    if (!studentId || !entryTime) {
        showNotification('Please select student and entry time!', 'error');
        return;
    }
    
    try {
        const response = await fetch('/api/attendance', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                student_id: parseInt(studentId),
                entry_time: entryTime,
                exit_time: exitTime || null,
                date: new Date().toISOString().split('T')[0],
                bus_number: userInfo.bus_number
            })
        });
        
        if (response.ok) {
            showNotification('Attendance marked!', 'success');
            document.getElementById('verifyStudent').value = '';
            document.getElementById('exitTime').value = '';
            setCurrentTime();
            await loadAttendance();
        } else {
            showNotification('Failed to mark attendance!', 'error');
        }
    } catch (error) {
        showNotification('Connection error!', 'error');
    }
}

// Load attendance
async function loadAttendance() {
    const container = document.getElementById('todayAttendance');
    
    try {
        const today = new Date().toISOString().split('T')[0];
        const promises = students.map(s => 
            fetch(`/api/attendance?student_id=${s.id}&date=${today}`).then(r => r.json())
        );
        
        const results = await Promise.all(promises);
        const allRecords = results.flat();
        
        if (allRecords.length === 0) {
            container.innerHTML = '<p class="empty-state">No attendance records for today</p>';
            return;
        }
        
        container.innerHTML = allRecords.map(record => `
            <div class="attendance-item">
                <h4>👦 ${record.student_name}</h4>
                <p>⏰ Entry: ${record.entry_time || 'Not marked'}</p>
                <p>⏰ Exit: ${record.exit_time || 'Not marked'}</p>
            </div>
        `).join('');
    } catch (error) {
        container.innerHTML = '<p class="empty-state">Error loading attendance</p>';
    }
}

// Initialize on load
window.addEventListener('DOMContentLoaded', init);
