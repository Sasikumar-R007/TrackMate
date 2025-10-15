#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* serverURL = "https://your-app-name.onrender.com/api/esp32/scan";
const char* apiKey = "YOUR_UNIQUE_SECRET_KEY_HERE";

const char* busNumber = "Bus-1";

#define ENTRY_BUTTON_PIN 13
#define EXIT_BUTTON_PIN 12

HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(115200);
  
  pinMode(ENTRY_BUTTON_PIN, INPUT_PULLUP);
  pinMode(EXIT_BUTTON_PIN, INPUT_PULLUP);
  
  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  
  Serial.println("TrackMate ESP32 Starting...");
  
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found!");
  } else {
    Serial.println("Fingerprint sensor not found!");
    while (1) { delay(1); }
  }
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  static bool entryMode = false;
  static bool exitMode = false;
  
  if (digitalRead(ENTRY_BUTTON_PIN) == LOW) {
    entryMode = true;
    exitMode = false;
    Serial.println("ENTRY MODE - Please scan fingerprint");
    delay(500);
  }
  
  if (digitalRead(EXIT_BUTTON_PIN) == LOW) {
    exitMode = true;
    entryMode = false;
    Serial.println("EXIT MODE - Please scan fingerprint");
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
    return 255;
  }
  
  Serial.print("Found ID #");
  Serial.println(finger.fingerID);
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
        Serial.println("✓ " + String((const char*)responseDoc["message"]));
        Serial.println("Student: " + String((const char*)responseDoc["student_name"]));
        Serial.println("Time: " + String((const char*)responseDoc["time"]));
      } else {
        Serial.println("✗ " + String((const char*)responseDoc["message"]));
      }
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
  }
}
