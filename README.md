# 🚌 TrackMate - School Van Attendance System

An interactive, colorful web-based attendance tracking system designed for school van management with role-based access for teachers and parents.

## ✨ Features

### 🎨 Interactive Design
- **Colorful & Animated UI**: Beautiful gradient backgrounds with floating animated shapes
- **Bus Animation**: Animated school bus with rotating wheels on login page
- **Responsive Design**: Works perfectly on mobile and desktop devices
- **Child-Friendly**: Bright colors, emojis, and engaging animations

### 👥 User Roles

#### 🎓 Teachers
- **Enroll Students**: Add students with fingerprint IDs
- **Mark Attendance**: Record entry and exit times
- **Student Management**: View and search all students on assigned bus
- **Real-time Updates**: See today's attendance records instantly

#### 👨‍👩‍👧 Parents
- **Bus Information**: View bus details, driver info, and teacher contacts
- **Attendance Tracking**: Check child's daily entry/exit times
- **History Access**: View attendance records for any date
- **Student Profile**: See complete student and bus information

### 🔐 Authentication
- Secure login system with password hashing
- Separate registration for teachers and parents
- Session-based authentication
- Role-based dashboard access

### 🚌 Bus Management
- **Interconnected System**: Bus number automatically fills bus route
- **Pre-configured Routes**:
  - Bus-1: Route A - Main Street → School
  - Bus-2: Route B - Park Avenue → School
  - Bus-3: Route C - Lake Road → School
  - Bus-4: Route D - Hill Street → School
- Driver and teacher assignment tracking

## 🚀 Quick Start

The application is already configured and running! Simply:

1. Click the **Run** button or use the webview
2. Register as a teacher or parent
3. Access your personalized dashboard

### First Time Setup

**For Teachers:**
1. Click "Register" on the login page
2. Select "Teacher" tab
3. Fill in your details (name, mobile, password)
4. Select your class and bus number
5. Bus route will auto-fill
6. Login and start managing students!

**For Parents:**
1. Click "Register" on the login page
2. Select "Parent" tab
3. Fill in your details and student information
4. Select bus number (route auto-fills)
5. Login to view bus details and attendance!

## 💻 Technical Stack

- **Backend**: Python Flask with RESTful API
- **Database**: SQLite with automated schema setup
- **Frontend**: Vanilla JavaScript with modern CSS animations
- **Authentication**: Werkzeug password hashing with session management
- **Server**: Runs on port 5000 with CORS enabled

## 📁 Project Structure

```
TrackMate/
├── app.py                          # Flask backend with API endpoints
├── trackmate.db                    # SQLite database (auto-created)
├── static/                         # Frontend files
│   ├── login.html                 # Animated login/register page
│   ├── teacher-dashboard.html     # Teacher dashboard
│   ├── parent-dashboard.html      # Parent dashboard
│   ├── styles.css                 # Login page styles
│   ├── dashboard.css              # Dashboard styles
│   ├── login.js                   # Login functionality
│   ├── teacher-dashboard.js       # Teacher dashboard logic
│   └── parent-dashboard.js        # Parent dashboard logic
└── trackmate.ino                  # Arduino code (ESP8266 hardware)
```

## 📊 Database Schema

### Users Table
- Authentication credentials for teachers and parents
- Links to bus assignments and class information

### Students Table
- Student details linked to parent accounts
- Bus assignment and fingerprint ID tracking

### Bus Details Table
- Bus information with driver details
- Teacher assignments and capacity

### Attendance Table
- Daily entry/exit time records
- Date-based tracking for historical queries

## 🎯 Usage Examples

### Teacher Dashboard
1. **Enroll a Student**: Enter name, roll number, class, and fingerprint ID
2. **Mark Attendance**: Select student, enter entry/exit times
3. **View Students**: See all students with search functionality

### Parent Dashboard
1. **Check Bus Details**: View driver and teacher information
2. **Today's Attendance**: See if child boarded and exited the bus
3. **Attendance History**: Select any date to view past records

## 🔧 API Endpoints

- `POST /api/register` - Register new user (teacher/parent)
- `POST /api/login` - User login
- `POST /api/logout` - User logout
- `GET /api/user-info` - Get current user information
- `GET /api/bus-routes` - Get all bus routes
- `GET /api/bus-details/<bus_number>` - Get specific bus details
- `GET /api/students` - Get students list
- `GET /api/attendance` - Get attendance records
- `POST /api/attendance` - Mark attendance

## 🛠️ Hardware Integration (Optional)

The `trackmate.ino` file contains Arduino code for ESP8266 to interface with:
- Adafruit fingerprint sensor
- SSD1306 OLED display (128x64)
- WiFi connectivity for real-time updates

## 🚢 Deployment

This project is configured for easy deployment on Replit:
- Click the **Deploy** button to publish
- Get a public URL for your school
- Automatic scaling based on usage

## 📝 Notes

- Default bus details and routes are pre-configured in the database
- Sessions are secure with generated secret keys
- All passwords are hashed using Werkzeug security
- The system supports multiple teachers and parents
- Attendance tracking includes both entry and exit times

## 🎨 Design Highlights

- **Gradient Backgrounds**: Purple and blue gradients throughout
- **Floating Shapes**: Animated circles that move across the screen
- **Smooth Transitions**: All interactions have smooth animations
- **Emoji Icons**: Visual indicators for better user experience
- **Color-Coded Status**: Green for present, red for absent
- **Responsive Cards**: Information displayed in attractive gradient cards

---

Built with ❤️ for schools to track student attendance safely and efficiently!
