// ========================= LIBRARIES =========================

#include <WiFi.h> 
#include <WebServer.h>
#include <Preferences.h> //Stores last activity in flash memory (CRAZY NO?)
#include <ArduinoJson.h> // Parses JSON (english) file for hardware

Preferences prefs;  // For saving schedules persistently
WebServer server(80);

// ========================= CONFIG =========================
const char* ssid     = "ESP32-MediSafe";
const char* password = "12345678";
// ===========================================================

// Your HTML page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>MediSafe AI – Scheduler</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 40px; }
    input[type="text"] { width: 100px; margin-right: 10px; }
    input[type="time"] { width: 120px; margin-right: 10px; }
    button { margin: 5px; padding: 8px 12px; }
    #scheduleList { width: 300px; margin: 20px auto; text-align: left; border: 1px solid #ccc; padding: 10px; }
    #saveBtn { background-color: green; color: white; font-weight: bold; border: none; }
  </style>
</head>
<body>
  <h2>MediSafe AI – Scheduler</h2>
  <input type="time" id="timeInput">
  <input type="text" id="medicineInput" placeholder="Medicine">
  <br>
  <button onclick="addSchedule()">Add</button>
  <button onclick="editSchedule()">Edit</button>
  <button onclick="deleteSchedule()">Delete</button>

  <div id="scheduleList"></div>

  <button id="saveBtn" onclick="saveAndStart()">✅ Save & Start</button>

  <script>
    let schedules = [];
    let selectedIndex = -1;

    function renderList() {
      const listDiv = document.getElementById("scheduleList");
      listDiv.innerHTML = "";
      schedules.forEach((item, index) => {
        const div = document.createElement("div");
        div.textContent = `${item.time} – ${item.medicine}`;
        div.style.cursor = "pointer";
        div.onclick = () => selectItem(index);
        if (index === selectedIndex) {
          div.style.backgroundColor = "#ddd";
        }
        listDiv.appendChild(div);
      });
    }

    function selectItem(index) {
      selectedIndex = index;
      document.getElementById("timeInput").value = schedules[index].time;
      document.getElementById("medicineInput").value = schedules[index].medicine;
      renderList();
    }

    function addSchedule() {
      const time = document.getElementById("timeInput").value;
      const medicine = document.getElementById("medicineInput").value;
      if (time && medicine) {
        schedules.push({ time, medicine });
        renderList();
      }
    }

    function editSchedule() {
      if (selectedIndex >= 0) {
        schedules[selectedIndex] = {
          time: document.getElementById("timeInput").value,
          medicine: document.getElementById("medicineInput").value
        };
        renderList();
      }
    }

    function deleteSchedule() {
      if (selectedIndex >= 0) {
        schedules.splice(selectedIndex, 1);
        selectedIndex = -1;
        renderList();
      }
    }

    function saveAndStart() {
      fetch("/save", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(schedules)
      }).then(res => {
        if (res.ok) alert("Schedules Saved!");
        else alert("Error saving schedules.");
      });
    }
  </script>
</body>
</html>
)rawliteral";

// Store schedules in memory
String schedulesData = "[]";

// ===== Serve HTML =====
void handleRoot() {
  server.send_P(200, "text/html", index_html); //Sends HTML code stored in index_html 
}

// ===== Handle Saving =====
void handleSave() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Invalid Request");
    return;
  }

  String jsonData = server.arg("plain");
  schedulesData = jsonData;

  // Validate JSON
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, jsonData);
  if (error) {
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  // Save persistently
  prefs.begin("medisafe", false);
  prefs.putString("schedules", schedulesData);
  prefs.end();

  Serial.println("Schedules saved:");
  serializeJsonPretty(doc, Serial);

  server.send(200, "text/plain", "Saved");
}

// ===== Load Schedules from Flash =====
void loadSchedulesFromFlash() {
  prefs.begin("medisafe", true);
  schedulesData = prefs.getString("schedules", "[]");
  prefs.end();

  Serial.println("Loaded schedules from memory:");
  Serial.println(schedulesData);
}

void setup() {
  Serial.begin(115200);

  // WiFi Setup (AP Mode)
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Load previous schedules
  loadSchedulesFromFlash();

  // Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
