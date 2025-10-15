# TrackMate - School Van Attendance System

## Overview
TrackMate is an interactive, colorful fingerprint-based attendance tracking system designed specifically for school van management. The system features separate dashboards for teachers and parents with role-based access control.

## Recent Changes
- **October 15, 2025**: Complete redesign with interactive UI, login system, and role-based dashboards
  - Added colorful, animated login page with floating shapes and bus animation
  - Implemented Flask backend with SQLite database
  - Created separate teacher and parent dashboards
  - Added bus route and number interconnection
  - Implemented attendance tracking with entry/exit times

## Project Structure

### Frontend Files (static/)
- `login.html` - Animated login/registration page with teacher and parent options
- `teacher-dashboard.html` - Teacher dashboard with enroll, verify, and student list
- `parent-dashboard.html` - Parent dashboard with bus details and attendance tracking
- `styles.css` - Colorful, animated styles for login page
- `dashboard.css` - Dashboard styles with responsive design
- `login.js` - Login/registration logic with form validation
- `teacher-dashboard.js` - Teacher dashboard functionality
- `parent-dashboard.js` - Parent dashboard functionality

### Backend Files
- `app.py` - Flask application with API endpoints for authentication and data management
- `trackmate.db` - SQLite database (auto-created)

### Legacy Files
- `index.html`, `verify.html`, `students.html` - Original static pages (kept for reference)
- `trackmate.ino` - Arduino code for ESP8266 with fingerprint sensor
- `server.py` - Legacy simple HTTP server (replaced by app.py)

## Features

### Authentication System
- **Teacher Login/Registration**: 
  - Name, mobile, password
  - Class assignment (dropdown)
  - Student count
  - Bus number (dropdown) - auto-fills bus route
  - Bus route (auto-populated)

- **Parent Login/Registration**:
  - Parent name, mobile, password
  - Student name, roll number
  - Class selection
  - Bus number and route (interconnected)

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

### Parent Dashboard
1. **Bus Details Section**
   - Bus number and route information
   - Driver name and contact
   - Teacher name and contact
   - Student information card

2. **Attendance Section**
   - Today's attendance with entry/exit times
   - Historical attendance lookup by date
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
- Responsive design for mobile and desktop
- Smooth transitions and hover effects
- Emoji icons for visual appeal

## Technical Setup
- **Backend**: Flask with session-based authentication
- **Database**: SQLite for easy deployment
- **Frontend**: Vanilla JavaScript with modern CSS animations
- **Server**: Runs on port 5000

## User Preferences
- Target audience: School students and parents
- Design: Colorful, animated, child-friendly
- Features: Role-based access, attendance tracking, bus information management

## Configuration
- Server host: 0.0.0.0 (accessible from network)
- Server port: 5000
- Database: SQLite (trackmate.db)
- Sessions: Secure cookie-based with generated secret key
