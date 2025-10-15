// Bus routes mapping
const busRoutes = {
    'Bus-1': 'Route A - Main Street → School',
    'Bus-2': 'Route B - Park Avenue → School',
    'Bus-3': 'Route C - Lake Road → School',
    'Bus-4': 'Route D - Hill Street → School'
};

// Show notification
function showNotification(message, type = 'success') {
    const notification = document.getElementById('notification');
    notification.textContent = message;
    notification.className = `notification ${type} show`;
    
    setTimeout(() => {
        notification.classList.remove('show');
    }, 3000);
}

// Toggle between login and register
function toggleForm(formType) {
    const loginForm = document.getElementById('loginForm');
    const registerForm = document.getElementById('registerForm');
    
    if (formType === 'login') {
        loginForm.classList.remove('hidden');
        registerForm.classList.add('hidden');
    } else {
        loginForm.classList.add('hidden');
        registerForm.classList.remove('hidden');
    }
}

// Show tab in login
function showTab(userType) {
    const teacherLogin = document.getElementById('teacherLogin');
    const parentLogin = document.getElementById('parentLogin');
    const tabs = document.querySelectorAll('#loginForm .tab-btn');
    
    tabs.forEach(tab => tab.classList.remove('active'));
    event.target.classList.add('active');
    
    if (userType === 'teacher') {
        teacherLogin.classList.remove('hidden');
        parentLogin.classList.add('hidden');
    } else {
        teacherLogin.classList.add('hidden');
        parentLogin.classList.remove('hidden');
    }
}

// Show tab in register
function showRegisterTab(userType) {
    const teacherRegister = document.getElementById('teacherRegister');
    const parentRegister = document.getElementById('parentRegister');
    const tabs = document.querySelectorAll('#registerForm .tab-btn');
    
    tabs.forEach(tab => tab.classList.remove('active'));
    event.target.classList.add('active');
    
    if (userType === 'teacher') {
        teacherRegister.classList.remove('hidden');
        parentRegister.classList.add('hidden');
    } else {
        teacherRegister.classList.add('hidden');
        parentRegister.classList.remove('hidden');
    }
}

// Update bus route based on bus number
function updateBusRoute(busNumber, routeFieldId) {
    const routeField = document.getElementById(routeFieldId);
    routeField.value = busRoutes[busNumber] || '';
}

// Login function
async function login(userType) {
    const password = document.getElementById(`${userType}Password`).value;
    let loginData = { user_type: userType, password };
    
    if (userType === 'teacher') {
        const name = document.getElementById('teacherName').value;
        if (!name || !password) {
            showNotification('Please fill all fields!', 'error');
            return;
        }
        loginData.name = name;
    } else {
        const rollNumber = document.getElementById('parentRollNumber').value;
        if (!rollNumber || !password) {
            showNotification('Please fill all fields!', 'error');
            return;
        }
        loginData.roll_number = rollNumber;
    }
    
    try {
        const response = await fetch('/api/login', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(loginData)
        });
        
        const data = await response.json();
        
        if (data.success) {
            showNotification(`Welcome ${data.name}!`, 'success');
            setTimeout(() => {
                window.location.href = data.user_type === 'teacher' ? 'teacher-dashboard.html' : 'parent-dashboard.html';
            }, 1000);
        } else {
            showNotification(data.message || 'Login failed!', 'error');
        }
    } catch (error) {
        showNotification('Connection error!', 'error');
    }
}

// Register function
async function register(userType) {
    let data = { user_type: userType };
    
    if (userType === 'teacher') {
        data.name = document.getElementById('regTeacherName').value;
        data.mobile = document.getElementById('regTeacherMobile').value;
        data.password = document.getElementById('regTeacherPassword').value;
        data.class = document.getElementById('regTeacherClass').value;
        data.students_count = document.getElementById('regTeacherStudents').value;
        data.bus_number = document.getElementById('regTeacherBus').value;
        data.bus_route = document.getElementById('regTeacherRoute').value;
    } else {
        data.name = document.getElementById('regParentName').value;
        data.mobile = document.getElementById('regParentMobile').value;
        data.password = document.getElementById('regParentPassword').value;
        data.student_name = document.getElementById('regStudentName').value;
        data.roll_number = document.getElementById('regRollNumber').value;
        data.class = document.getElementById('regParentClass').value;
        data.bus_number = document.getElementById('regParentBus').value;
        data.bus_route = document.getElementById('regParentRoute').value;
    }
    
    // Validate
    for (let key in data) {
        if (!data[key]) {
            showNotification('Please fill all fields!', 'error');
            return;
        }
    }
    
    try {
        const response = await fetch('/api/register', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });
        
        const result = await response.json();
        
        if (result.success) {
            showNotification('Registration successful! Please login.', 'success');
            setTimeout(() => toggleForm('login'), 1500);
        } else {
            showNotification(result.message || 'Registration failed!', 'error');
        }
    } catch (error) {
        showNotification('Connection error!', 'error');
    }
}

// Handle Enter key
document.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') {
        const activeTab = document.querySelector('.tab-btn.active');
        if (activeTab) {
            const userType = activeTab.textContent.includes('Teacher') ? 'teacher' : 'parent';
            if (document.getElementById('loginForm').classList.contains('hidden')) {
                register(userType);
            } else {
                login(userType);
            }
        }
    }
});
