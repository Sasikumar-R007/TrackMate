#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WebServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* serverURL = "https://YOUR_RENDER_URL.onrender.com/api/esp32/scan";
const char* apiKey = "YOUR_API_KEY_HERE";

const char* busNumber = "Bus-1";

HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

WebServer webServer(80);

String currentMode = "entry";
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

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial;max-width:500px;margin:50px auto;padding:20px;background:#f0f0f0;}";
  html += "h1{color:#333;text-align:center;}";
  html += "button{width:100%;padding:20px;margin:10px 0;font-size:18px;border:none;border-radius:8px;cursor:pointer;}";
  html += ".entry{background:#4CAF50;color:white;}.exit{background:#f44336;color:white;}";
  html += ".status{background:#2196F3;color:white;padding:15px;border-radius:8px;text-align:center;margin:20px 0;}";
  html += "</style></head><body>";
  html += "<h1>🚌 TrackMate Control</h1>";
  html += "<div class='status'>Current Mode: <b>" + String(currentMode == "entry" ? "ENTRY" : "EXIT") + "</b></div>";
  html += "<button class='entry' onclick=\"location.href='/mode/entry'\">📥 Set ENTRY Mode</button>";
  html += "<button class='exit' onclick=\"location.href='/mode/exit'\">📤 Set EXIT Mode</button>";
  html += "<p style='text-align:center;color:#666;'>Bus: " + String(busNumber) + "</p>";
  html += "</body></html>";
  
  webServer.send(200, "text/html", html);
}

void handleMode() {
  if (webServer.pathArg(0) == "entry") {
    currentMode = "entry";
    showMessage("Mode Changed", "ENTRY MODE", "Scan Finger");
  } else if (webServer.pathArg(0) == "exit") {
    currentMode = "exit";
    showMessage("Mode Changed", "EXIT MODE", "Scan Finger");
  }
  
  webServer.sendHeader("Location", "/");
  webServer.send(302);
}

void handleStatus() {
  StaticJsonDocument<200> doc;
  doc["mode"] = currentMode;
  doc["bus"] = busNumber;
  doc["wifi"] = WiFi.SSID();
  doc["ip"] = WiFi.localIP().toString();
  
  String json;
  serializeJson(doc, json);
  webServer.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED display not found!"));
    while (1);
  }
  
  display.clearDisplay();
  showMessage("TrackMate", "No-Button Mode", "Initializing...");
  delay(1000);
  
  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  
  Serial.println("TrackMate ESP32 Starting (No Button Mode)...");
  
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
  
  webServer.on("/", handleRoot);
  webServer.on("/mode/entry", handleMode);
  webServer.on("/mode/exit", handleMode);
  webServer.on("/status", handleStatus);
  webServer.begin();
  
  Serial.println("Web server started at: http://" + WiFi.localIP().toString());
  
  showMessage("TrackMate Ready", "Mode: " + String(currentMode).toUpperCase(), WiFi.localIP().toString());
  delay(2000);
  showMessage("ENTRY MODE", "Scan Finger", WiFi.localIP().toString());
}

void loop() {
  webServer.handleClient();
  
  if (millis() - lastScanTime > scanCooldown) {
    uint8_t fingerprintID = getFingerprintID();
    
    if (fingerprintID != 255) {
      sendToServer(fingerprintID, currentMode);
      lastScanTime = millis();
      
      delay(2000);
      showMessage(String(currentMode).toUpperCase() + " MODE", "Scan Finger", WiFi.localIP().toString());
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
        String time = String((const char*)responseDoc["time"]);
        
        Serial.println("✓ " + message);
        Serial.println("Student: " + studentName);
        Serial.println("Time: " + time);
        
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
