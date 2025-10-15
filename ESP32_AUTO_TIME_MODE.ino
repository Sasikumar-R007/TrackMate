#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* serverURL = "https://YOUR_RENDER_URL.onrender.com/api/esp32/scan";
const char* apiKey = "YOUR_API_KEY_HERE";

const char* busNumber = "Bus-1";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

const int ENTRY_HOUR_START = 6;
const int ENTRY_HOUR_END = 9;

HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

unsigned long lastScanTime = 0;
const unsigned long scanCooldown = 3000;

void showMessage(String line1, String line2 = "", String line3 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.setCursor(0, 10);
  display.println(line1);
  
  if (line2.length() > 0) {
    display.setCursor(0, 25);
    display.println(line2);
  }
  
  if (line3.length() > 0) {
    display.setCursor(0, 40);
    display.println(line3);
  }
  
  display.display();
}

String getCurrentMode() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "entry";
  }
  
  int hour = timeinfo.tm_hour;
  
  if (hour >= ENTRY_HOUR_START && hour < ENTRY_HOUR_END) {
    return "entry";
  } else if (hour >= 14 && hour < 17) {
    return "exit";
  } else {
    return "entry";
  }
}

String getCurrentTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "??:??";
  }
  
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  return String(timeStr);
}

void setup() {
  Serial.begin(115200);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED display not found!"));
    while (1);
  }
  
  display.clearDisplay();
  showMessage("TrackMate", "Auto-Time Mode", "Initializing...");
  delay(1000);
  
  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  
  Serial.println("TrackMate ESP32 Starting (Auto Time Mode)...");
  
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found!");
    showMessage("TrackMate", "Sensor: OK");
  } else {
    Serial.println("Fingerprint sensor not found!");
    showMessage("ERROR!", "Sensor Not Found", "Check Wiring");
    while (1) { delay(1); }
  }
  
  delay(1000);
  showMessage("Connecting to", "WiFi...");
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  showMessage("WiFi Connected!", "Syncing Time...");
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  delay(2000);
  
  String mode = getCurrentMode();
  String currentTime = getCurrentTime();
  
  showMessage(mode.toUpperCase() + " MODE", "Time: " + currentTime, "Scan Finger");
  
  Serial.println("Current mode: " + mode + " (based on time: " + currentTime + ")");
}

void loop() {
  if (millis() - lastScanTime > scanCooldown) {
    uint8_t fingerprintID = getFingerprintID();
    
    if (fingerprintID != 255) {
      String mode = getCurrentMode();
      String currentTime = getCurrentTime();
      
      Serial.println("Auto-detected mode: " + mode + " (Time: " + currentTime + ")");
      
      sendToServer(fingerprintID, mode);
      lastScanTime = millis();
      
      delay(2000);
      showMessage(mode.toUpperCase() + " MODE", "Time: " + currentTime, "Scan Finger");
    }
  }
  
  delay(50);
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return 255;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return 255;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    return 255;
  }
  
  Serial.print("Found ID #");
  Serial.println(finger.fingerID);
  showMessage("ID Found!", "ID: " + String(finger.fingerID), "Sending...");
  return finger.fingerID;
}

void sendToServer(uint8_t fingerprintID, String scanType) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-API-Key", apiKey);
    
    StaticJsonDocument<200> doc;
    doc["fingerprint_id"] = fingerprintID;
    doc["bus_number"] = busNumber;
    doc["scan_type"] = scanType;
    
    String jsonData;
    serializeJson(doc, jsonData);
    
    Serial.println("Sending to server: " + jsonData);
    
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
      
      StaticJsonDocument<300> responseDoc;
      deserializeJson(responseDoc, response);
      
      if (responseDoc["success"]) {
        String message = String((const char*)responseDoc["message"]);
        String studentName = String((const char*)responseDoc["student_name"]);
        
        Serial.println("✓ " + message);
        Serial.println("Student: " + studentName);
        
        showMessage("SUCCESS!", studentName, message);
      } else {
        String errorMsg = String((const char*)responseDoc["message"]);
        Serial.println("✗ " + errorMsg);
        showMessage("ERROR!", errorMsg);
      }
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
      showMessage("Server Error!", "Code: " + String(httpResponseCode));
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
    showMessage("WiFi Error!", "Disconnected");
  }
}
