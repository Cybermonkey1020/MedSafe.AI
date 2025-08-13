#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>

#define BUTTON1 2
#define BUTTON2 16

const char* ssid = "iPhone_13(MYNK)";
const char* password = "12345678";
char formattedTimeStr[30]; 
struct tm timeinfo;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800; // GMT+5:30 in seconds for IST
const int daylightOffset_sec = 0; // No daylight saving for India

// Google Apps Script Web App URL
const char* serverName = "https://script.google.com/macros/s/AKfycbwNPOxWf8AU5l84nHr6_0kGnOhp8NebiTAL3PmGlNm2nX3KeLh2_IbaG0lsPEsYef2HIg/exec";

void setup(){
   Serial.begin(115200);

   // Button Setup
   pinMode(BUTTON1, INPUT_PULLUP); 
   pinMode(BUTTON2, INPUT_PULLUP);
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
  if (!getLocalTime(&timeinfo)) {
   Serial.println("Failed to obtain time");
   delay(2000);
   return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void loop(){
   if(WiFi.status() == WL_CONNECTED){
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");

      // Get latest time before sending
      if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time for upload. Skipping this upload."); // Changed message
      delay(5000); // Wait before trying again
      return; 
      }

      strftime(formattedTimeStr, sizeof(formattedTimeStr), "%H:%M:%S", &timeinfo);
      Serial.print("Current time for upload: ");
      Serial.println(formattedTimeStr); 
      String formattedTime = String(formattedTimeStr);

      // Prepare JSON payload with SNTP time and button state
      String jsonData = "{\"method\":\"append\","
                      "\"med1\":true,"
                      "\"med2\":false,"
                      "\"med3\":true,"
                      "\"timestamp\":\"" + formattedTime + "\","
                      "\"buttonState\":true}";

      Serial.print("Sending JSON: ");
      Serial.println(jsonData);

      // Send HTTP POST request
      int httpResponseCode = http.POST(jsonData);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      } else {
        Serial.println("Error on sending POST: " + String(httpResponseCode));
      }
      http.end(); // Close connection
    }
   else {
      Serial.println("WiFi Disconnected");
    }                  
   delay(3000);
}