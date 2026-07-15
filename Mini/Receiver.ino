#include <WiFi.h>
#include <esp_now.h>

#define RED_LED     18
#define GREEN_LED   19
#define BUZZER      23

SemaphoreHandle_t alertSemaphore;

typedef struct {
  char command[10];
} Message;

Message incomingMsg;
bool alertState = false;

void AlarmTask(void *pvParameters)
{
  while (1)
  {
    xSemaphoreTake(alertSemaphore, portMAX_DELAY);

    if (alertState)
    {
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(BUZZER, HIGH);

      Serial.println("ALERT MODE");
    }
    else
    {
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(BUZZER, LOW);

      Serial.println("SAFE MODE");
    }
  }
}

void OnDataRecv(const esp_now_recv_info_t *info,
                const uint8_t *incomingData,
                int len)
{
  memcpy(&incomingMsg, incomingData, sizeof(incomingMsg));

  if (strcmp(incomingMsg.command, "ALERT") == 0)
  {
    alertState = true;
  }
  else if (strcmp(incomingMsg.command, "SAFE") == 0)
  {
    alertState = false;
  }

  xSemaphoreGive(alertSemaphore);
}

void setup()
{
  Serial.begin(115200);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, HIGH);

  WiFi.mode(WIFI_STA);

  esp_now_init();

  esp_now_register_recv_cb(OnDataRecv);

  alertSemaphore = xSemaphoreCreateBinary();

  xTaskCreate(
      AlarmTask,
      "Alarm Task",
      2048,
      NULL,
      1,
      NULL);
}

void loop()
{
}