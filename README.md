# TrackMate - Fingerprint Attendance System

A fingerprint-based attendance tracking system designed for bus attendance management.

## 🚀 Quick Start

The project is already configured and running! Simply click the **Run** button or use the webview to access the application.

## 📋 Features

- **Student Enrollment**: Register students with fingerprint data
- **Fingerprint Verification**: Verify student identity using fingerprints
- **Student Management**: View all enrolled students
- **Firebase Integration**: Real-time data synchronization

## 🌐 Web Pages

1. **Enrollment Page** (`/` or `index.html`)
   - Register new students with ID and name
   - Enroll fingerprints via ESP8266 device

2. **Verification Page** (`verify.html`)
   - Verify student fingerprints
   - Records attendance automatically

3. **Students List** (`students.html`)
   - View all enrolled students
   - Displays data from Firebase Realtime Database

## 🔧 Technical Setup

### Components
- **Frontend**: Static HTML/CSS/JavaScript pages
- **Server**: Python HTTP server (port 5000)
- **Hardware**: ESP8266 with fingerprint sensor (optional, for hardware integration)
- **Database**: Firebase Realtime Database

### Configuration

The project uses:
- Firebase Database: `trackmate-07-default-rtdb.firebaseio.com`
- Bus ID: "Bus1"
- ESP8266 IP addresses (configured in HTML files for local network)

## 🛠️ Hardware Integration (Optional)

The `trackmate.ino` file contains Arduino code for ESP8266 to:
- Interface with Adafruit fingerprint sensor
- Display status on OLED screen (128x64 SSD1306)
- Connect to WiFi and Firebase
- Provide web interface for enrollment

### Hardware Requirements
- ESP8266 microcontroller
- Adafruit fingerprint sensor
- SSD1306 OLED display
- WiFi network

## 📝 Notes

- The web interface will attempt to connect to ESP8266 device for fingerprint operations
- Update ESP IP addresses in the HTML files to match your local network setup
- Firebase already contains sample student data
- The server includes cache control headers to ensure fresh content delivery

## 🚢 Deployment

This project is configured for Replit's autoscale deployment. Click the **Deploy** button when ready to publish your application with a public URL.
