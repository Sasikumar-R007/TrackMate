#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* serverURL = "https://YOUR_REPLIT_URL.replit.dev/api/esp32/scan";
const char* apiKey = "YOUR_API_KEY_HERE";

const char* busNumber = "Bus-1";

#define ENTRY_BUTTON_PIN 13
#define EXIT_BUTTON_PIN 12

HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

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

void setup() {
  Serial.begin(115200);
  
  pinMode(ENTRY_BUTTON_PIN, INPUT_PULLUP);
  pinMode(EXIT_BUTTON_PIN, INPUT_PULLUP);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED display not found!"));
    while (1);
  }
  
  display.clearDisplay();
  showMessage("TrackMate", "Initializing...");
  delay(1000);
  
  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  
  Serial.println("TrackMate ESP32 Starting...");
  
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
  
  showMessage("WiFi Connected!", WiFi.localIP().toString());
  delay(2000);
  
  showMessage("TrackMate Ready", "Press Button", "Then Scan");
}

void loop() {
  static bool entryMode = false;
  static bool exitMode = false;
  
  if (digitalRead(ENTRY_BUTTON_PIN) == LOW) {
    entryMode = true;
    exitMode = false;
    Serial.println("ENTRY MODE - Please scan fingerprint");
    showMessage("ENTRY MODE", "Place Finger", "on Sensor");
    delay(500);
  }
  
  if (digitalRead(EXIT_BUTTON_PIN) == LOW) {
    exitMode = true;
    entryMode = false;
    Serial.println("EXIT MODE - Please scan fingerprint");
    showMessage("EXIT MODE", "Place Finger", "on Sensor");
    delay(500);
  }
  
  if (entryMode || exitMode) {
    uint8_t fingerprintID = getFingerprintID();
    
    if (fingerprintID != 255) {
      String scanType = entryMode ? "entry" : "exit";
      sendToServer(fingerprintID, scanType);
      
      entryMode = false;
      exitMode = false;
      delay(3000);
      showMessage("TrackMate Ready", "Press Button", "Then Scan");
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
    Serial.println("Fingerprint not found!");
    showMessage("Not Found!", "Try Again");
    delay(2000);
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
      showMessage("Server Error!", "Code: " + String(httpResponseCode), "Check Connection");
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
    showMessage("WiFi Error!", "Disconnected", "Check Network");
  }
}
