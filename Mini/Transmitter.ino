#include <esp_now.h>
#include <WiFi.h>

uint8_t receiverMAC[] = {0x24,0x6F,0x28,0xAA,0xBB,0xCC};   // Replace with receiver MAC

#define BUTTON_PIN 4

typedef struct {
  char command[10];
} Message;

Message msg;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);
}

void loop() {

  if (!digitalRead(BUTTON_PIN)) {

    strcpy(msg.command, "ALERT");

    esp_now_send(receiverMAC, (uint8_t *)&msg, sizeof(msg));

    delay(500);
  }
}