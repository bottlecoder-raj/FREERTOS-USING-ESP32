#include <esp_now.h>
#include <WiFi.h>

uint8_t receiverMAC[] = {0x8C, 0x4B, 0x14, 0x4B, 0x2E, 0x44};

#define BUTTON_PIN 25

typedef struct {
  char command[10];
} Message;

Message msg;

bool alertState = false;      // Current mode
bool lastButtonState = HIGH;  // Previous button state



void setup()
{
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW Init Failed");
    return;
  }


  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Sender Ready");
}

void loop()
{
  bool buttonState = digitalRead(BUTTON_PIN);

  // Detect button press (falling edge)
  if (lastButtonState == HIGH && buttonState == LOW)
  {
    alertState = !alertState;   // Toggle state

    if (alertState)
    {
      strcpy(msg.command, "ALERT");
      Serial.println("Sending ALERT");
    }
    else
    {
      strcpy(msg.command, "SAFE");
      Serial.println("Sending SAFE");
    }

    esp_err_t result = esp_now_send(receiverMAC,
                                    (uint8_t *)&msg,
                                    sizeof(msg));

    if (result == ESP_OK)
      Serial.println("Message Queued");
    else
      Serial.println("Send Error");

    delay(200);   // Simple debounce
  }

  lastButtonState = buttonState;
}