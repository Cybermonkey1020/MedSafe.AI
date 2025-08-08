#include <WiFi.h>
#include <WebServer.h>

int num_val;
#define ledPin 8

const char* ssid = "ESP - 32 (WiFi_Mayank)";
const char* password = "123";



WebServer Server(80);  //Create a web server with address 80 (Arbitary)

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
  Server.send(200, "text/html", htmlpage);
}

void slider_input() {
  if (Server.hasArg("value")) {
    num_val = Server.arg("value").toInt();
    Server.send(200, "text/plain", "Value  -" + String(num_val));
    Serial.println(num_val);
  } else {
    Server.send(400, "text/plain", "Missing value");
  }
}

void setup() {
  Serial.begin(115200);
  // pinMode(ledPin, OUTPUT);
  WiFi.softAP(ssid, password);
  Serial.println("WIFI ACCESS pOINT ON");
  Serial.println(WiFi.softAPIP());
  while (!WiFi.status()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  Server.begin();
  Server.on("/", handleRoot);
  Server.on("/setSpeed", slider_input);
}

void loop() {
  Server.handleClient();
}