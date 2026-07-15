#include <WiFi.h>
#include <esp_now.h>

//----------------------- Pins -----------------------
#define START_BTN  18
#define NEXT_BTN   19
#define BUZZER     23

//--------------------- Message ----------------------
typedef enum
{
  GO,
  BUZZ,
  LOCK,
  RESET
} MessageType;

typedef struct
{
  MessageType type;
  uint8_t playerID;
} Packet;

Packet txPacket;
Packet rxPacket;

//--------------- FreeRTOS Objects -------------------
SemaphoreHandle_t startSemaphore;
SemaphoreHandle_t nextSemaphore;
QueueHandle_t playerQueue;

//------------------- Variables ----------------------
bool gameStarted = false;
bool winnerDeclared = false;
uint8_t winnerID = 0;

//----------------------------------------------------
// Replace with your broadcast peer or send individually
//----------------------------------------------------
uint8_t player1[] = {...};
uint8_t player2[] = {...};
uint8_t player3[] = {...};
uint8_t player4[] = {...};
uint8_t player5[] = {...};
//----------------------------------------------------
void beep(int times)
{
  for(int i=0;i<times;i++)
  {
    digitalWrite(BUZZER,HIGH);
    vTaskDelay(pdMS_TO_TICKS(200));
    digitalWrite(BUZZER,LOW);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

//----------------------------------------------------
void broadcast(Packet pkt)
{
    esp_now_send(player1, (uint8_t*)&pkt, sizeof(pkt));
    esp_now_send(player2, (uint8_t*)&pkt, sizeof(pkt));
    esp_now_send(player3, (uint8_t*)&pkt, sizeof(pkt));
    esp_now_send(player4, (uint8_t*)&pkt, sizeof(pkt));
    esp_now_send(player5, (uint8_t*)&pkt, sizeof(pkt));
}

//----------------------------------------------------
void IRAM_ATTR startISR()
{
  BaseType_t hpTask = pdFALSE;

  xSemaphoreGiveFromISR(startSemaphore,&hpTask);

  portYIELD_FROM_ISR(hpTask);
}

//----------------------------------------------------
void IRAM_ATTR nextISR()
{
  BaseType_t hpTask = pdFALSE;

  xSemaphoreGiveFromISR(nextSemaphore,&hpTask);

  portYIELD_FROM_ISR(hpTask);
}

//----------------------------------------------------
void OnDataRecv(const esp_now_recv_info_t *info,
                const uint8_t *incomingData,
                int len)
{
  memcpy(&rxPacket,incomingData,sizeof(rxPacket));

  if(gameStarted &&
     !winnerDeclared &&
     rxPacket.type==BUZZ)
  {
      xQueueSend(playerQueue,&rxPacket.playerID,0);
  }
}

//----------------------------------------------------
void QuizTask(void *pvParameters)
{
  uint8_t player;

  while(1)
  {
      Serial.println("\nWaiting for START...");

      xSemaphoreTake(startSemaphore,portMAX_DELAY);

      Serial.println("Question Started");

      beep(3);

      gameStarted = true;
      winnerDeclared = false;

      sendPacket(GO,0);

      Serial.println("GO Sent");

      xQueueReceive(playerQueue,&player,portMAX_DELAY);

      winnerID = player;

      winnerDeclared = true;
      gameStarted = false;

      Serial.print("Winner : Player ");
      Serial.println(winnerID);

      beep(1);

      sendPacket(LOCK,winnerID);

      Serial.println("LOCK Sent");

      xSemaphoreTake(nextSemaphore,portMAX_DELAY);

      sendPacket(RESET,0);

      Serial.println("RESET Sent");
  }
}

//----------------------------------------------------
void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER,OUTPUT);

  pinMode(START_BTN,INPUT_PULLUP);
  pinMode(NEXT_BTN,INPUT_PULLUP);

  attachInterrupt(START_BTN,startISR,FALLING);
  attachInterrupt(NEXT_BTN,nextISR,FALLING);

  startSemaphore = xSemaphoreCreateBinary();
  nextSemaphore = xSemaphoreCreateBinary();

  playerQueue = xQueueCreate(5,sizeof(uint8_t));

  WiFi.mode(WIFI_STA);

  if(esp_now_init()!=ESP_OK)
  {
      Serial.println("ESP NOW Init Failed");
      return;
  }

  esp_now_peer_info_t peerInfo={};

  memcpy(peerInfo.peer_addr,peerAddress,6);

  peerInfo.channel=0;
  peerInfo.encrypt=false;

  esp_now_add_peer(&peerInfo);

  esp_now_register_recv_cb(OnDataRecv);

  xTaskCreate(
      QuizTask,
      "QuizTask",
      4096,
      NULL,
      2,
      NULL
  );
}

//----------------------------------------------------
void loop()
{
}