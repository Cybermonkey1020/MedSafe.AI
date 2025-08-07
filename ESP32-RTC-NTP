#include <WiFi.h>
#include "time.h"
#include "sntp.h"

// Wi-Fi credentials
const char* ssid      = "YOUR_SSID";
const char* password  = "YOUR_PASSWORD";

// NTP servers
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";

// Time zone offset for IST (GMT+5:30)
const long gmtOffset_sec = 19800;  // 5.5 hours = 19800 seconds
const int daylightOffset_sec = 0;  // No daylight saving in India

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  // Format: HH:MM:SS
  Serial.println(&timeinfo, "%H:%M:%S");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  // Configure time with NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  // Wait until time is synced
  Serial.println("Waiting for time...");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(500);
  }

  printLocalTime();  // Print time once after syncing
}

void loop() {
  printLocalTime();  // Continuously print time
  delay(1000);
}
