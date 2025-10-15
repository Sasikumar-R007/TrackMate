# TrackMate - School Van Attendance System

## Overview
TrackMate is an interactive, colorful fingerprint-based attendance tracking system designed specifically for school van management. The system features separate dashboards for teachers and parents with role-based access control.

## Recent Changes
- **October 15, 2025 - Latest Update**: Enhanced login system and dashboard improvements
  - **Login Credentials Changed**:
    - Teachers login with their name and password (not mobile)
    - Parents login with student roll number and password (not mobile)
  - **Teacher Dashboard**: Added "Teacher Details" section for profile management
  - **Parent Dashboard Enhancements**:
    - Phone call button with icon for direct teacher contact
    - Edit student details feature (name and class)
    - Improved attendance history with compact date selector
  
- **October 15, 2025 - Initial Version**: Complete redesign with interactive UI
  - Added colorful, animated login page with floating shapes and bus animation
  - Implemented Flask backend with SQLite database
  - Created separate teacher and parent dashboards
  - Added bus route and number interconnection
  - Implemented attendance tracking with entry/exit times

## Project Structure

### Frontend Files (static/)
- `login.html` - Animated login/registration page with teacher and parent options
- `teacher-dashboard.html` - Teacher dashboard with enroll, verify, student list, and teacher details
- `parent-dashboard.html` - Parent dashboard with bus details, attendance tracking, and edit features
- `styles.css` - Colorful, animated styles for login page
- `dashboard.css` - Dashboard styles with responsive design and new components
- `login.js` - Login/registration logic with updated credential system
- `teacher-dashboard.js` - Teacher dashboard functionality with profile updates
- `parent-dashboard.js` - Parent dashboard with call buttons and edit features

### Backend Files
- `app.py` - Flask application with API endpoints for authentication and data management
- `trackmate.db` - SQLite database (auto-created)

### Legacy Files
- `trackmate.ino` - Arduino code for ESP8266 with fingerprint sensor

## Features

### Authentication System
- **Teacher Login**: 
  - Login with teacher name and password
  - Registration includes: name, mobile, password, class, student count, bus number, bus route

- **Parent Login**:
  - Login with student roll number and password
  - Registration includes: parent name, mobile, password, student name, roll number, class, bus number, bus route

### Teacher Dashboard
1. **Enroll Student Section**
   - Add new students with name, roll number, class
   - Assign fingerprint ID
   - Auto-assign to teacher's bus

2. **Mark Attendance Section**
   - Select student from dropdown
   - Record entry and exit times
   - View today's attendance records

3. **Student List Section**
   - View all students on assigned bus
   - Search functionality
   - Colorful student cards with details

4. **Teacher Details Section** (NEW)
   - View and update teacher name
   - Update mobile number
   - View assigned class, bus number, and route

### Parent Dashboard
1. **Bus Details Section**
   - Bus number and route information
   - Driver name and contact
   - Teacher information with phone call button
   - Student information card

2. **Student Information** (ENHANCED)
   - View student details
   - Edit student name and class
   - Save and cancel options

3. **Attendance Section** (IMPROVED)
   - Today's attendance with entry/exit times
   - Compact date selector for historical lookup
   - Color-coded attendance status (green for present, red for absent)

## Database Schema

### Tables
- **users**: Teacher and parent accounts with authentication
- **students**: Student information linked to parents
- **bus_details**: Bus information with driver and teacher details
- **attendance**: Daily attendance records with entry/exit times

## Bus Route Mapping
- Bus-1: Route A - Main Street → School
- Bus-2: Route B - Park Avenue → School
- Bus-3: Route C - Lake Road → School
- Bus-4: Route D - Hill Street → School

## Design Features
- Animated gradient backgrounds with floating shapes
- Bus animation with rotating wheels
- Colorful gradient buttons and cards
- Phone call buttons with icons
- Edit forms with save/cancel options
- Compact date selector for better UX
- Responsive design for mobile and desktop
- Smooth transitions and hover effects
- Emoji icons for visual appeal

## Technical Setup
- **Backend**: Flask with session-based authentication
- **Database**: SQLite for easy deployment
- **Frontend**: Vanilla JavaScript with modern CSS animations
- **Server**: Runs on port 5000

## API Endpoints
- `POST /api/register` - Register new user
- `POST /api/login` - User login (name for teachers, roll number for parents)
- `POST /api/logout` - User logout
- `GET /api/user-info` - Get current user information
- `POST /api/update-teacher` - Update teacher details
- `POST /api/update-student` - Update student details
- `GET /api/bus-routes` - Get all bus routes
- `GET /api/bus-details/<bus_number>` - Get specific bus details
- `GET /api/students` - Get students list
- `GET /api/attendance` - Get attendance records
- `POST /api/attendance` - Mark attendance

## User Preferences
- Target audience: School students and parents
- Design: Colorful, animated, child-friendly
- Features: Role-based access, attendance tracking, bus information management, direct contact options

## Configuration
- Server host: 0.0.0.0 (accessible from network)
- Server port: 5000
- Database: SQLite (trackmate.db)
- Sessions: Secure cookie-based with generated secret key
