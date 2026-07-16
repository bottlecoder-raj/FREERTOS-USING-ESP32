#include <WiFi.h>

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.print("ESP32 MAC Address: ");
  WiFi.mode(WIFI_STA);
  
  delay(1000);
  Serial.println(WiFi.macAddress());
}

void loop() {
}