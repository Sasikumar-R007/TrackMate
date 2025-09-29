#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>

// ------------------ Global Objects ------------------
WiFiClientSecure client; // HTTPS client for Firebase

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SoftwareSerial fingerSerial(D6, D7);
Adafruit_Fingerprint finger(&fingerSerial);

ESP8266WebServer server(80);

// ------------------ WiFi & Firebase ------------------
const char* ssid = "Sasikumar R";
const char* password = "sasikumar";
const String firebaseHost = "trackmate-07-default-rtdb.firebaseio.com";
const String busID = "Bus1";

String currentMode = "verify"; // default mode

// ------------------ Helper Functions ------------------
void showMessage(String line1, String line2="") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println(line1);
  if(line2.length()>0){
    display.setCursor(0,25);
    display.println(line2);
  }
  display.display();
}

String getDate(){ return "2025-09-28"; }
String getTime(){ return "12:00:00"; }

// ------------------ Firebase ------------------
void sendToFirebase(String path, String json) {
  if(WiFi.status()==WL_CONNECTED){
    HTTPClient http;
    String url = "https://" + firebaseHost + "/" + path + ".json";
    client.setInsecure(); // allow HTTPS
    http.begin(client, url);
    http.addHeader("Content-Type","application/json");
    int code = http.PUT(json);
    Serial.println("Firebase PUT response: " + String(code));
    http.end();
  }
}

String getFromFirebase(String path){
  String payload="";
  if(WiFi.status()==WL_CONNECTED){
    HTTPClient http;
    String url = "https://" + firebaseHost + "/" + path + ".json";
    client.setInsecure();
    http.begin(client, url);
    int code = http.GET();
    if(code==200) payload = http.getString();
    http.end();
  }
  return payload;
}

// ------------------ Fingerprint ------------------
void enrollFingerWeb(int id, String name){
  showMessage("Enroll Mode","Place Finger");
  int p=-1;
  while(p!=FINGERPRINT_OK)p=finger.getImage();
  p=finger.image2Tz(1);
  if(p!=FINGERPRINT_OK) return;

  showMessage("Remove Finger");
  delay(2000);

  showMessage("Place Again");
  p=-1; while(p!=FINGERPRINT_OK)p=finger.getImage();
  p=finger.image2Tz(2);
  if(p!=FINGERPRINT_OK) return;

  p=finger.createModel();
  if(p!=FINGERPRINT_OK){showMessage("Error:Mismatch"); return;}
  p=finger.storeModel(id);
  if(p!=FINGERPRINT_OK){showMessage("Store Failed"); return;}

  String json = "{\"id\":"+String(id)+",\"name\":\""+name+"\",\"fingerprintStored\":true,\"bus\":\""+busID+"\"}";
  sendToFirebase("students/"+String(id),json);
  showMessage("Enrolled Successfully","ID:"+String(id));
}

void verifyFinger(){
  int p = finger.getImage();
  if(p==FINGERPRINT_OK){
    p=finger.image2Tz();
    if(p==FINGERPRINT_OK){
      p=finger.fingerFastSearch();
      if(p==FINGERPRINT_OK){
        int id = finger.fingerID;
        String json = getFromFirebase("students/"+String(id));
        if(json!="null"){
          DynamicJsonDocument doc(256);
          deserializeJson(doc,json);
          String name = doc["name"];
          showMessage("Access Granted",name);

          String att="{\"id\":"+String(id)+",\"name\":\""+name+"\",\"date\":\""+getDate()+"\",\"time\":\""+getTime()+"\",\"bus\":\""+busID+"\"}";
          sendToFirebase("attendance/"+String(id)+"_"+String(millis()),att);
          delay(2000);
        } else showMessage("Access Denied");
      } else showMessage("Access Denied");
    }
  }
}

// ------------------ Web Page ------------------
const char webPageHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>TrackMate</title>
<style>
body{font-family:Arial;background:#f4f4f4;margin:0;padding:0;}
header{background:#007BFF;color:#fff;padding:15px;text-align:center;font-size:1.5em;}
.container{max-width:500px;margin:20px auto;background:#fff;padding:20px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.2);}
select,input,button{width:100%;padding:10px;margin:8px 0;border-radius:5px;border:1px solid #ccc;font-size:1em;}
button{background:#007BFF;color:#fff;border:none;cursor:pointer;}
button:hover{background:#0056b3;}
#status{margin-top:15px;padding:10px;background:#eee;border-radius:5px;text-align:center;}
</style>
</head>
<body>
<header>TrackMate – Bus Attendance</header>
<div class="container">
<label for="mode">Mode:</label>
<select id="mode">
<option value="verify">Verify</option>
<option value="enroll">Enroll</option>
</select>
<input type="number" id="studentID" placeholder="Student ID">
<input type="text" id="studentName" placeholder="Student Name">
<button onclick="enroll()">Enroll Fingerprint</button>
<div id="status">Status: Waiting...</div>
<script>
const modeSelect=document.getElementById('mode');
const statusDiv=document.getElementById('status');
modeSelect.addEventListener('change',()=>{fetch(`/mode?set=${modeSelect.value}`).then(r=>r.text()).then(d=>statusDiv.innerText="Status: "+d);});
function enroll(){const id=document.getElementById('studentID').value;const name=document.getElementById('studentName').value;if(!id||!name){alert("Enter ID and Name!");return;}fetch(`/enroll?id=${id}&name=${name}`).then(r=>r.text()).then(d=>statusDiv.innerText="Status: "+d);}
</script>
</div>
</body>
</html>
)rawliteral";

// ------------------ Web Handlers ------------------
void handleRoot(){ server.send_P(200, "text/html", webPageHTML); }
void handleMode(){ 
  if(server.hasArg("set")){
    currentMode = server.arg("set");
    server.send(200, "text/plain", "Mode set to " + currentMode);
  } else server.send(200, "text/plain", "Current mode: " + currentMode);
}
void handleEnroll(){
  if(server.hasArg("id") && server.hasArg("name")){
    int id = server.arg("id").toInt();
    String name = server.arg("name");
    enrollFingerWeb(id, name);
    server.send(200, "text/plain", "Enrolled ID "+String(id));
  } else server.send(400, "text/plain","Missing ID or Name");
}

// ------------------ Setup & Loop ------------------
void setup() {
  Serial.begin(115200);
  fingerSerial.begin(57600);

  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)) while(1);
  display.clearDisplay();
  showMessage("TrackMate","Initializing...");

  if(finger.verifyPassword()) showMessage("Sensor Found");
  else { showMessage("Sensor NOT Found"); while(1); }

  WiFi.begin(ssid,password);
  showMessage("Connecting WiFi...");
  while(WiFi.status()!=WL_CONNECTED){delay(500);}
  showMessage("WiFi Connected",WiFi.localIP().toString());

  client.setInsecure(); // HTTPS without cert check

  // Start web server
  server.on("/", handleRoot);
  server.on("/mode", handleMode);
  server.on("/enroll", handleEnroll);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
  if(currentMode=="verify") verifyFinger();
  delay(200);
}
