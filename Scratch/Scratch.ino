#include <WiFi.h>
#include "time.h"

const char* ssid = "iPhone_13(MYNK)";
const char* password = "12345678";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800; // GMT+5:30 in seconds for IST
const int daylightOffset_sec = 0; // No daylight saving for India

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Configure and initialize SNTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Print the local time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void loop() {
  // You don't need to call update() repeatedly.
  // The SNTP service runs in the background.
  delay(5000);

  // You can check the time whenever you need it
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%H:%M:%S");
}