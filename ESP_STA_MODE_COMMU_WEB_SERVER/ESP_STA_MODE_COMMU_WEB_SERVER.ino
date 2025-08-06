#include <WiFi.h>
#include <WebServer.h>

int light;
#define ledPin 8

const char* ssid = "ESP - 32 (WiFi)";
const char* password = "123";

WebServer server(80); //Create a web server with address 80 (Arbitary)

const char htmlpage[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html>
  <body>
    <h2>Motor Speed Control</h2>
    <label for="speedSlider">Speed:</label>
    <input type="range" min="0" max="255" value="128" id="speedSlider" oninput="updateSpeed(this.value)">
    <span id="valueLabel">128</span>

    <script>
      function updateSpeed(val) {
        document.getElementById("valueLabel").innerText = val;
        fetch("/setSpeed?value=" + val)  // Send value to ESP32
          .then(response => response.text())
          .then(data => console.log(data));
      }
    </script>
  </body>
</html>
)rawliteral";
void handleRoot() {
  server.send(200, "text/html",htmlpage);
}

void slider_input(){
    if(server.hasArg("value")){
       light = server.arg("value").toInt(); 
       constrain(light, 0, 255);
       analogWrite(ledPin, light);
    }
}

void setup() {
  Serial.begin(115200);
  Serial.println(WiFi.softAPIP());
  // ledcSetup(0, 5000, 8);  // Channel 0, 5 kHz, 8-bit resolution
  // ledcAttachPin(ledPin, 0); 
  pinMode(ledPin, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.begin();
  server.on("/", handleRoot);
  server.on("/setSpeed", slider_input);
}

void loop() {
  server.handleClient();
}