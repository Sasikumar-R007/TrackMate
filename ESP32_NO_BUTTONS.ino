#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Wire.h>

// ===================== CONFIG =====================
const char* ssid = "Sasikumar R";
const char* password = "sasikumar";

const char* serverURL = "https://trackmate-cwo1.onrender.com/api/esp32/scan";
const char* apiKey = "YOUR_API_KEY_HERE";
const char* busNumber = "Bus-1";

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Fingerprint sensor
SoftwareSerial mySerial(D6, D7); // RX, TX
Adafruit_Fingerprint finger(&mySerial);

// Web server
ESP8266WebServer webServer(80);

// Mode and timing
String currentMode = "ENTRY";
unsigned long lastScanTime = 0;
const unsigned long scanCooldown = 3000;

// ===================== FUNCTIONS =====================
void showMessage(String line1, String line2 = "", String line3 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(line1);
  display.setCursor(0, 20);
  display.println(line2);
  display.setCursor(0, 40);
  display.println(line3);
  display.display();
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return 255;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return 255;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return 255;

  Serial.print("Fingerprint found: ID ");
  Serial.println(finger.fingerID);
  showMessage("ID Found!", "ID: " + String(finger.fingerID), "Sending...");
  return finger.fingerID;
}

void sendToServer(uint8_t fingerprintID, String scanType) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;

    http.begin(client, serverURL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-API-Key", apiKey);

    StaticJsonDocument<200> doc;
    doc["fingerprint_id"] = fingerprintID;
    doc["bus_number"] = busNumber;
    doc["scan_type"] = scanType;

    String jsonData;
    serializeJson(doc, jsonData);

    Serial.println("Sending: " + jsonData);
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
      showMessage("Sent ✅", "ID: " + String(fingerprintID), "Type: " + scanType);
    } else {
      Serial.println("Error sending data: " + String(httpResponseCode));
      showMessage("Server ❌", "Code: " + String(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi disconnected!");
    showMessage("WiFi ❌", "Disconnected");
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>TrackMate</title></head><body><h1>TrackMate</h1>";
  html += "<p>Current Mode: " + currentMode + "</p>";
  html += "<a href='/mode/entry'>Set ENTRY</a> | <a href='/mode/exit'>Set EXIT</a>";
  html += "</body></html>";
  webServer.send(200, "text/html", html);
}

void handleMode() {
  String path = webServer.uri();
  if (path.endsWith("/entry")) {
    currentMode = "ENTRY";
    showMessage("Mode Changed", "ENTRY MODE", "Scan Finger");
  } else if (path.endsWith("/exit")) {
    currentMode = "EXIT";
    showMessage("Mode Changed", "EXIT MODE", "Scan Finger");
  }
  webServer.sendHeader("Location", "/");
  webServer.send(302);
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  mySerial.begin(57600);
  Wire.begin(D2, D1); // OLED SDA, SCL

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while(1);
  display.clearDisplay();
  showMessage("TrackMate", "Initializing...");

  finger.begin(57600);
  if(finger.verifyPassword()){
    Serial.println("Fingerprint sensor detected ✅");
    showMessage("Sensor ✅", "Ready");
  } else {
    Serial.println("Fingerprint sensor NOT found ❌");
    showMessage("Sensor ❌", "Check wiring");
    while(1) delay(1);
  }

  WiFi.begin(ssid, password);
  showMessage("Connecting WiFi...", ssid);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected: " + WiFi.localIP().toString());
  showMessage("WiFi ✅", WiFi.localIP().toString(), "Mode: " + currentMode);

  webServer.on("/", handleRoot);
  webServer.on("/mode/entry", handleMode);
  webServer.on("/mode/exit", handleMode);
  webServer.begin();
  Serial.println("Web server started");
}

// ===================== LOOP =====================
void loop() {
  webServer.handleClient();

  if(millis() - lastScanTime > scanCooldown) {
    uint8_t id = getFingerprintID();
    if(id != 255){
      sendToServer(id, currentMode);
      lastScanTime = millis();
      delay(2000);
      showMessage(currentMode + " MODE", "Scan Finger", WiFi.localIP().toString());
    }
  }
}
