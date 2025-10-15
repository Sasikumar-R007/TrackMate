# TrackMate - Fingerprint Attendance System

## Overview
TrackMate is a fingerprint-based attendance tracking system designed for bus attendance management. The system consists of:
- **Frontend**: Static HTML pages for enrollment, verification, and student management
- **Hardware**: ESP8266-based fingerprint sensor with OLED display
- **Backend**: Firebase Realtime Database for data storage

## Project Structure
- `index.html` - Student enrollment page
- `verify.html` - Fingerprint verification page
- `students.html` - View all enrolled students
- `trackmate.ino` - Arduino code for ESP8266 with fingerprint sensor
- `server.py` - Python HTTP server to serve the frontend

## Setup Date
- Initial setup: October 15, 2025
- Configured for Replit environment with Python 3.11

## Configuration
- Frontend server runs on port 5000
- Uses Firebase Realtime Database: `trackmate-07-default-rtdb.firebaseio.com`
- Default Bus ID: "Bus1"

## Hardware Requirements (for .ino file)
- ESP8266 microcontroller
- Adafruit fingerprint sensor
- SSD1306 OLED display (128x64)

## Notes
- The HTML files connect to an ESP8266 device and Firebase
- ESP IP addresses in HTML files need to be updated based on local network setup
- Firebase URL is already configured in students.html
- The .ino file contains the ESP8266 firmware code (not runnable in Replit)
