# ESP32 + Fingerprint Sensor Integration Guide

## 📦 Hardware Requirements

1. **ESP32 Development Board** (any variant)
2. **R307/R305 Fingerprint Sensor Module** (or compatible)
3. **2 Push Buttons** (for Entry/Exit mode)
4. **Jumper Wires**
5. **Breadboard** (optional)
6. **Micro USB Cable** (for ESP32 power & programming)

---

## 🔌 Hardware Connections

### Fingerprint Sensor to ESP32:
```
Sensor Pin → ESP32 Pin
VCC (Red)  → 3.3V or 5V
GND (Black)→ GND
TX (Yellow)→ GPIO 16 (RX2)
RX (White) → GPIO 17 (TX2)
```

### Buttons to ESP32:
```
Entry Button → GPIO 13 → GND
Exit Button  → GPIO 12 → GND
```

*(Buttons use internal pull-up resistors)*

---

## 💻 Software Setup

### Step 1: Install Arduino IDE
1. Download from [arduino.cc](https://www.arduino.cc/en/software)
2. Install and open Arduino IDE

### Step 2: Add ESP32 Board Support
1. File → Preferences
2. In "Additional Board Manager URLs" add:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
3. Tools → Board → Boards Manager
4. Search "ESP32" and install **"ESP32 by Espressif Systems"**

### Step 3: Install Required Libraries
1. Sketch → Include Library → Manage Libraries
2. Search and install:
   - **Adafruit Fingerprint Sensor Library**
   - **ArduinoJson** (by Benoit Blanchon)

### Step 4: Generate Secure API Key

**CRITICAL SECURITY STEP** - Generate a unique secret API key for your hardware:

1. Visit [random.org/strings](https://www.random.org/strings/) or use this command:
   ```bash
   openssl rand -hex 32
   ```
   This generates a random 64-character key like: `a7f3d9e2b4c8...`

2. **Add to Render Environment Variables:**
   - Go to your Render dashboard
   - Select your TrackMate web service
   - Go to "Environment" tab
   - Click "Add Environment Variable"
   - Key: `HARDWARE_API_KEY`
   - Value: Paste your generated key
   - Click "Save Changes"
   - Wait for automatic redeploy (~2 minutes)

3. **IMPORTANT:** Keep this key secret! Don't share it or commit it to GitHub.

### Step 5: Configure the ESP32 Code
1. Open `ESP32_CODE.ino` in Arduino IDE
2. Update these lines with your info:

```cpp
const char* ssid = "YOUR_WIFI_NAME";              // Your WiFi name
const char* password = "YOUR_WIFI_PASSWORD";      // Your WiFi password
const char* serverURL = "https://trackmate.onrender.com/api/esp32/scan";  // Your Render URL
const char* apiKey = "PASTE_YOUR_GENERATED_KEY_HERE";  // The key from Step 4
const char* busNumber = "Bus-1";                  // Which bus this device is for
```

**Security Note:** The same API key must be in both Render (environment variable) and ESP32 code!

### Step 6: Upload Code
1. Connect ESP32 to computer via USB
2. Tools → Board → ESP32 Dev Module
3. Tools → Port → Select your COM port
4. Click Upload button (→)
5. Wait for "Done uploading"

---

## 👆 Enroll Fingerprints

Before using the system, you need to register student fingerprints:

### Option 1: Use Adafruit Example
1. File → Examples → Adafruit Fingerprint Sensor Library → **enroll**
2. Upload to ESP32
3. Open Serial Monitor (115200 baud)
4. Follow prompts to enroll fingerprints
5. **Important:** Note which ID number you assign to each student

### Option 2: Download Enrollment App
Many fingerprint sensors come with Windows software for enrollment.

### Student ID Mapping
In your TrackMate web dashboard:
1. Register a student
2. In the database, update their `fingerprint_id` field to match the enrolled ID
3. For example: If you enrolled "John Doe" as ID #5 in the sensor, set his `fingerprint_id = 5`

---

## 🚀 How to Use

### Daily Operation:

1. **Power on ESP32** (via USB or power bank)
2. Wait for "WiFi Connected!" message
3. **For Bus Entry:**
   - Press ENTRY button
   - Student scans fingerprint
   - LED blinks & attendance recorded
4. **For Bus Exit:**
   - Press EXIT button
   - Student scans fingerprint
   - Exit time recorded

### Check Attendance:
- Parents/Teachers log in to the web app
- View real-time attendance on dashboard
- See entry & exit times for each student

---

## 🔧 Troubleshooting

### WiFi Won't Connect
- Check SSID and password are correct
- Ensure ESP32 is in range of WiFi
- Try 2.4GHz network (ESP32 doesn't support 5GHz)

### Fingerprint Sensor Not Found
- Check wiring connections
- Try swapping TX/RX wires
- Ensure sensor has power (red LED should glow)

### Server Communication Fails
- Verify server URL is correct (should end with `/api/esp32/scan`)
- Check API key matches in both ESP32 code and web app
- Ensure Render app is deployed and running

### Fingerprint Not Recognized
- Re-enroll fingerprint with better quality
- Clean the sensor surface
- Ensure finger is dry and pressed firmly

---

## 🎯 Testing the System

### Test API Endpoint (Using Postman or curl):

```bash
curl -X POST https://your-app.onrender.com/api/esp32/scan \
  -H "Content-Type: application/json" \
  -H "X-API-Key: trackmate-esp32-2024" \
  -d '{
    "fingerprint_id": 1,
    "bus_number": "Bus-1",
    "scan_type": "entry"
  }'
```

Expected response:
```json
{
  "success": true,
  "message": "Entry recorded",
  "student_name": "John Doe",
  "time": "2024-10-15 10:30:00"
}
```

---

## 📱 Advanced: Add LCD Display

Want to show student names on a display?

**Add 16x2 I2C LCD:**
```cpp
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// In setup():
lcd.init();
lcd.backlight();

// After successful scan:
lcd.clear();
lcd.print(responseDoc["student_name"]);
lcd.setCursor(0, 1);
lcd.print(responseDoc["message"]);
```

---

## 🔐 Security Notes

1. **API Key Protection:**
   - ✅ Use a strong, random API key (see Step 4 above)
   - ✅ Never use simple keys like "123456" or "password"
   - ✅ Don't share your .ino file publicly (contains API key)
   - ✅ Rotate the key periodically (every 3-6 months)

2. **HTTPS Encryption:**
   - ✅ All data is encrypted in transit (Render provides SSL)
   - ✅ Fingerprint data is never stored, only IDs

3. **WiFi Security:**
   - ✅ Use WPA2/WPA3 encrypted WiFi
   - ✅ Don't connect to public/open WiFi networks

4. **If API Key is Compromised:**
   - Generate a new random key
   - Update Render environment variable
   - Re-upload ESP32 code with new key
   - Old key becomes invalid immediately

---

## 📊 What Gets Recorded

Each fingerprint scan creates a database entry with:
- Student name
- Bus number  
- Entry time (when boarding)
- Exit time (when leaving)
- Date
- Automatic notification to parents (optional enhancement)

---

## ✅ Quick Checklist

- [ ] Hardware assembled and wired correctly
- [ ] ESP32 board support installed in Arduino IDE
- [ ] Libraries installed (Adafruit Fingerprint, ArduinoJson)
- [ ] WiFi credentials updated in code
- [ ] Server URL updated (your Render URL)
- [ ] Bus number configured
- [ ] Code uploaded successfully
- [ ] Fingerprints enrolled with IDs
- [ ] Student records updated with fingerprint_id
- [ ] Tested entry scan
- [ ] Tested exit scan
- [ ] Verified attendance in web dashboard

---

## 🎉 You're All Set!

Your TrackMate hardware is ready! Students can now scan fingerprints to mark attendance, and parents can monitor in real-time through the web app.

**Need help?** Check the Serial Monitor (Tools → Serial Monitor) for debug messages.
