# 🔧 Complete ESP32 Hardware Setup Guide
## TrackMate School Van Attendance System

---

## 📦 Hardware Components Required

1. **ESP32 Development Board** (any variant - DevKit, WROOM, etc.)
2. **R307/R305 Fingerprint Sensor Module** (or AS608)
3. **SSD1306 OLED Display** (128x64, I2C)
4. **2 Push Buttons** (for Entry/Exit)
5. **Jumper Wires** (Male-to-Male and Male-to-Female)
6. **Breadboard** (optional but recommended)
7. **Micro USB Cable** (for power & programming)

---

## 🔌 Hardware Connections Diagram

### Fingerprint Sensor (R307/R305) → ESP32
```
┌─────────────────┐
│ Fingerprint     │
│ Sensor          │
├─────────────────┤
│ VCC (Red)   ────┼──→ 5V (or 3.3V on ESP32)
│ GND (Black) ────┼──→ GND
│ TX  (Yellow)────┼──→ GPIO 16 (RX2)
│ RX  (White) ────┼──→ GPIO 17 (TX2)
└─────────────────┘
```

### OLED Display (I2C SSD1306) → ESP32
```
┌─────────────────┐
│ OLED Display    │
│ (128x64 I2C)    │
├─────────────────┤
│ VCC ────────────┼──→ 3.3V
│ GND ────────────┼──→ GND
│ SCL ────────────┼──→ GPIO 22 (default I2C SCL)
│ SDA ────────────┼──→ GPIO 21 (default I2C SDA)
└─────────────────┘
```

### Push Buttons → ESP32
```
Entry Button:           Exit Button:
┌────┐                 ┌────┐
│ ●  │                 │ ●  │
└┬──┬┘                 └┬──┬┘
 │  └──→ GND            │  └──→ GND
 └─────→ GPIO 13        └─────→ GPIO 12
 
(Uses internal pull-up resistors)
```

### Complete Wiring Table

| Component | Pin | → | ESP32 Pin |
|-----------|-----|---|-----------|
| **Fingerprint Sensor** | | | |
| VCC (Red) | Power | → | 5V or 3.3V |
| GND (Black) | Ground | → | GND |
| TX (Yellow) | Transmit | → | GPIO 16 (RX2) |
| RX (White) | Receive | → | GPIO 17 (TX2) |
| **OLED Display** | | | |
| VCC | Power | → | 3.3V |
| GND | Ground | → | GND |
| SCL | Clock | → | GPIO 22 |
| SDA | Data | → | GPIO 21 |
| **Buttons** | | | |
| Entry Button Pin 1 | Signal | → | GPIO 13 |
| Entry Button Pin 2 | Ground | → | GND |
| Exit Button Pin 1 | Signal | → | GPIO 12 |
| Exit Button Pin 2 | Ground | → | GND |

---

## 💻 Software Setup

### Step 1: Install Arduino IDE
1. Download from [arduino.cc/software](https://www.arduino.cc/en/software)
2. Install and open Arduino IDE

### Step 2: Add ESP32 Board Support
1. Go to **File → Preferences**
2. In "Additional Board Manager URLs" add:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
3. Go to **Tools → Board → Boards Manager**
4. Search for "ESP32" 
5. Install **"ESP32 by Espressif Systems"**
6. Click **Close**

### Step 3: Install Required Libraries
1. Go to **Sketch → Include Library → Manage Libraries**
2. Search and install each of these:
   - **Adafruit Fingerprint Sensor Library** (by Adafruit)
   - **ArduinoJson** (by Benoit Blanchon)
   - **Adafruit SSD1306** (by Adafruit)
   - **Adafruit GFX Library** (by Adafruit)

---

## 📝 Configure Your ESP32 Code

### Step 1: Open the Code
1. Download `ESP32_OLED_CODE.ino` from this project
2. Open it in Arduino IDE

### Step 2: Update Configuration
Replace these lines in the code:

```cpp
// WiFi credentials
const char* ssid = "YOUR_WIFI_NAME";              // Your WiFi network name
const char* password = "YOUR_WIFI_PASSWORD";      // Your WiFi password

// Server URL - YOUR REPLIT URL
const char* serverURL = "https://4b51ce65-b9fa-464c-819f-d1babc6d9fb2-00-2r7pir1wexqe4.spock.replit.dev/api/esp32/scan";

// API Key - Check your Replit Secrets
const char* apiKey = "PASTE_YOUR_API_KEY_HERE";   // From Replit Secrets

// Bus number for this device
const char* busNumber = "Bus-1";                  // Change to Bus-2, Bus-3, etc.
```

**Important Notes:**
- The server URL above is your current Replit development URL
- Your API key is stored in Replit Secrets (HARDWARE_API_KEY)
- Each bus should have its own ESP32 with a different `busNumber`

### Step 3: Upload Code to ESP32
1. Connect ESP32 to computer via USB cable
2. Go to **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
3. Go to **Tools → Port** → Select your COM port (e.g., COM3 or /dev/ttyUSB0)
4. Click the **Upload** button (→ arrow icon)
5. Wait for "Done uploading" message
6. Open **Tools → Serial Monitor** (set baud rate to 115200)

---

## 👆 Enroll Fingerprints

Before students can use the system, you need to register their fingerprints:

### Method 1: Using Adafruit Example (Recommended)
1. In Arduino IDE: **File → Examples → Adafruit Fingerprint Sensor Library → enroll**
2. Upload this sketch to ESP32
3. Open Serial Monitor (115200 baud)
4. Follow the prompts:
   - Type a fingerprint ID number (1-127)
   - Place finger when prompted
   - Remove and place again
   - Fingerprint stored!

### Method 2: Using Windows Software
Some fingerprint sensors include Windows enrollment software. Check your sensor package.

### Important: Link Fingerprints to Students
After enrolling fingerprints, you need to update student records:

1. **Log into TrackMate web app as a teacher**
2. **Enroll the student** in the "Enroll Student" section
3. **Note the student ID** from the database
4. **In your database**, update the student's `fingerprint_id` field to match the enrolled fingerprint ID

Example:
- Enrolled "John Doe" as Fingerprint ID #5 in sensor
- In database: Set John's `fingerprint_id = 5`

---

## 🚀 How to Use the System

### Daily Operation:

**1. Power On**
- Connect ESP32 to USB power (computer, power bank, or wall adapter)
- OLED will show "TrackMate Initializing..."
- Wait for "WiFi Connected!" message
- Display shows "TrackMate Ready, Press Button, Then Scan"

**2. Record Entry (Students Boarding Bus)**
- Press **ENTRY** button (GPIO 13)
- OLED shows "ENTRY MODE, Place Finger, on Sensor"
- Student places finger on sensor
- Display shows student name and "Entry recorded"
- Attendance automatically saved to database

**3. Record Exit (Students Leaving Bus)**
- Press **EXIT** button (GPIO 12)
- OLED shows "EXIT MODE, Place Finger, on Sensor"
- Student places finger on sensor
- Display shows student name and "Exit recorded"
- Exit time automatically updated

**4. View Attendance**
- Parents/Teachers log into web app
- See real-time entry and exit times
- View attendance history for any date

---

## 🔍 Troubleshooting

### OLED Display Not Working
- **Issue**: Display shows nothing or is blank
- **Fix**: 
  - Check I2C address (default is 0x3C, some use 0x3D)
  - Verify VCC is connected to 3.3V (not 5V)
  - Check SDA/SCL connections to GPIO 21/22
  - Run I2C scanner sketch to find address

### WiFi Won't Connect
- **Issue**: Stuck at "Connecting to WiFi..."
- **Fix**:
  - Verify SSID and password are correct
  - Use 2.4GHz WiFi (ESP32 doesn't support 5GHz)
  - Move ESP32 closer to router
  - Check if WiFi has special characters in password

### Fingerprint Sensor Not Found
- **Issue**: "Sensor Not Found" on OLED
- **Fix**:
  - Check all 4 wires are connected correctly
  - Try swapping TX and RX wires
  - Ensure sensor has power (LED should light up)
  - Use 5V instead of 3.3V for VCC
  - Check baud rate is 57600

### Fingerprint Not Recognized
- **Issue**: Shows "Not Found!" after scanning
- **Fix**:
  - Re-enroll fingerprint with better quality
  - Clean sensor surface with soft cloth
  - Press finger firmly and hold still
  - Ensure finger is dry (not wet or sweaty)
  - Try different finger angle

### Server Communication Error
- **Issue**: "Server Error!" on display
- **Fix**:
  - Verify serverURL matches your Replit URL exactly
  - Check API key matches the one in Replit Secrets
  - Ensure Replit app is running (not sleeping)
  - Check WiFi internet connection
  - Verify bus number exists in database

### Button Not Responding
- **Issue**: Pressing button does nothing
- **Fix**:
  - Check button connections to GPIO 13 and 12
  - Verify other pin is connected to GND
  - Test button with multimeter
  - Try different GPIO pins and update code

---

## 🧪 Testing the System

### Test 1: Hardware Check
Open Serial Monitor (115200 baud) and verify:
```
✓ TrackMate ESP32 Starting...
✓ Fingerprint sensor found!
✓ WiFi Connected!
✓ IP Address: 192.168.x.x
```

### Test 2: Button Test
- Press Entry button → Should see "ENTRY MODE - Please scan fingerprint"
- Press Exit button → Should see "EXIT MODE - Please scan fingerprint"

### Test 3: Fingerprint Test
- Press Entry button
- Scan enrolled finger
- Check Serial Monitor for:
```
✓ Found ID #5
✓ Sending to server: {"fingerprint_id":5,"bus_number":"Bus-1","scan_type":"entry"}
✓ Response: {"success":true,"message":"Entry recorded",...}
✓ Student: John Doe
```

### Test 4: OLED Display Test
OLED should show different messages:
- "TrackMate Ready" (idle)
- "ENTRY MODE" (when button pressed)
- "ID Found! ID: 5" (when fingerprint detected)
- "SUCCESS! John Doe" (when recorded)

### Test 5: Web Dashboard Test
1. Login to TrackMate web app as parent/teacher
2. Check attendance section
3. Verify entry time is recorded
4. Test exit scan and verify exit time appears

---

## 🎯 Multiple Bus Setup

If you have multiple buses, follow these steps for each bus:

1. **Assemble one ESP32 unit per bus**
2. **Upload code with different `busNumber`:**
   - Bus 1: `const char* busNumber = "Bus-1";`
   - Bus 2: `const char* busNumber = "Bus-2";`
   - Bus 3: `const char* busNumber = "Bus-3";`
   - Bus 4: `const char* busNumber = "Bus-4";`
3. **Enroll fingerprints separately** for each bus (students on Bus-1 get IDs 1-30, Bus-2 gets 31-60, etc.)
4. **Update database** with correct bus_number and fingerprint_id for each student

---

## 🔐 Security Best Practices

1. **API Key Protection**
   - ✅ Never share your API key publicly
   - ✅ Don't upload .ino files with API keys to GitHub
   - ✅ Use Replit Secrets to store the key
   - ✅ Rotate key every 3-6 months

2. **WiFi Security**
   - ✅ Use WPA2/WPA3 encrypted WiFi
   - ✅ Don't use public/open networks
   - ✅ Consider setting up a dedicated WiFi for devices

3. **Physical Security**
   - ✅ Mount ESP32 in secure enclosure
   - ✅ Keep sensor clean and protected
   - ✅ Lock buttons to prevent unauthorized access

---

## 📊 What Gets Recorded

Each fingerprint scan creates/updates a database record:

**Entry Scan:**
- Student ID and name
- Bus number
- Entry time (timestamp)
- Date
- Status: "Entry recorded"

**Exit Scan:**
- Updates existing entry record
- Adds exit time (timestamp)
- Status: "Exit recorded"

**Parents can view:**
- Real-time attendance status
- Entry and exit times
- Historical attendance records

---

## ✅ Quick Setup Checklist

- [ ] All hardware components purchased
- [ ] Wiring completed as per diagram
- [ ] Arduino IDE installed
- [ ] ESP32 board support added
- [ ] All 4 libraries installed
- [ ] WiFi credentials updated in code
- [ ] Server URL updated (your Replit URL)
- [ ] API key added (from Replit Secrets)
- [ ] Bus number configured
- [ ] Code uploaded to ESP32
- [ ] Serial Monitor shows successful connection
- [ ] Fingerprints enrolled using example sketch
- [ ] Student database updated with fingerprint IDs
- [ ] Entry scan tested successfully
- [ ] Exit scan tested successfully
- [ ] OLED display showing messages correctly
- [ ] Web dashboard showing attendance data

---

## 🎉 You're Ready!

Your TrackMate hardware system is complete! 

**Normal Operation Flow:**
1. Power on ESP32 in the morning
2. Press ENTRY button before students board
3. Each student scans fingerprint
4. Parents receive real-time updates
5. Press EXIT button when students leave
6. Each student scans fingerprint again
7. Complete attendance record created

**For Support:**
- Check Serial Monitor for debug messages
- Watch OLED display for error messages
- Review troubleshooting section above
- Verify all connections match the diagram

---

## 📱 Your Current System URLs

**Web Application:** https://4b51ce65-b9fa-464c-819f-d1babc6d9fb2-00-2r7pir1wexqe4.spock.replit.dev

**API Endpoint:** https://4b51ce65-b9fa-464c-819f-d1babc6d9fb2-00-2r7pir1wexqe4.spock.replit.dev/api/esp32/scan

**API Key Location:** Replit Secrets → HARDWARE_API_KEY

---

Happy Tracking! 🚌✨
