#include <WiFi.h>
#include <esp_now.h>

//----------------------- Pins -----------------------
#define START_BTN  18
#define NEXT_BTN   19
#define BUZZER     25
#define DEBOUNCE_DELAY 300  // 300ms debounce delay

volatile uint32_t lastStartTime = 0;
volatile uint32_t lastNextTime = 0;

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

Packet rxPacket;

//--------------- FreeRTOS Objects -------------------
SemaphoreHandle_t startSemaphore;
SemaphoreHandle_t nextSemaphore;
QueueHandle_t playerQueue;

//------------------- Variables ----------------------
bool gameStarted = false;
bool winnerDeclared = false;
uint8_t winnerID = 0;

//------------------ Player MACs ---------------------
// Replace these with your actual MAC addresses
uint8_t player1[] = {0x8C, 0x4B, 0x14, 0x4B, 0x18, 0xB0};
uint8_t player2[] = {0xD4, 0x8A, 0xFC, 0xD0, 0x76, 0x4C};
uint8_t player3[] = {0xBC, 0x4B, 0x14, 0x47, 0xBD, 0x48};
uint8_t player4[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x03};
uint8_t player5[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x04};
uint8_t player6[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x05};

//----------------------------------------------------
void beep(int times)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(BUZZER, HIGH);
    vTaskDelay(pdMS_TO_TICKS(200));
    digitalWrite(BUZZER, LOW);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

//----------------------------------------------------
void broadcast(Packet pkt)
{
  esp_now_send(player1, (uint8_t *)&pkt, sizeof(pkt));
  esp_now_send(player2, (uint8_t *)&pkt, sizeof(pkt));
  esp_now_send(player3, (uint8_t *)&pkt, sizeof(pkt));
  esp_now_send(player4, (uint8_t *)&pkt, sizeof(pkt));
  esp_now_send(player5, (uint8_t *)&pkt, sizeof(pkt));
  esp_now_send(player6, (uint8_t *)&pkt, sizeof(pkt));
}

//----------------------------------------------------
void sendPacket(MessageType type, uint8_t playerID)
{
  Packet pkt;
  pkt.type = type;
  pkt.playerID = playerID;

  broadcast(pkt);
}

//----------------------------------------------------
void IRAM_ATTR startISR()
{
  uint32_t currentTime = millis();
  // Only trigger if debounce delay has passed
  if (currentTime - lastStartTime > DEBOUNCE_DELAY)
  {
    BaseType_t hpTask = pdFALSE;
    xSemaphoreGiveFromISR(startSemaphore, &hpTask);
    lastStartTime = currentTime;
    portYIELD_FROM_ISR(hpTask);
  }
}

//----------------------------------------------------
void IRAM_ATTR nextISR()
{
  uint32_t currentTime = millis();
  // Only trigger if debounce delay has passed
  if (currentTime - lastNextTime > DEBOUNCE_DELAY)
  {
    BaseType_t hpTask = pdFALSE;
    xSemaphoreGiveFromISR(nextSemaphore, &hpTask);
    lastNextTime = currentTime;
    portYIELD_FROM_ISR(hpTask);
  }
}

//----------------------------------------------------
void OnDataRecv(const esp_now_recv_info_t *info,
                const uint8_t *incomingData,
                int len)
{
  if (len != sizeof(Packet))
    return;

  memcpy(&rxPacket, incomingData, sizeof(Packet));

  if (gameStarted &&
      !winnerDeclared &&
      rxPacket.type == BUZZ)
  {
    BaseType_t hpTask = pdFALSE;

    xQueueSendFromISR(playerQueue,
                      &rxPacket.playerID,
                      &hpTask);

    portYIELD_FROM_ISR(hpTask);
  }
}

//----------------------------------------------------
void QuizTask(void *pvParameters)
{
  uint8_t player;
  uint8_t dummy;

  while (1)
  {
    // STEP 1: Wait for START button only
    Serial.println("\n=== Waiting for START button ===");
    xSemaphoreTake(startSemaphore, portMAX_DELAY);

    // Clear old queue entries
    while (xQueueReceive(playerQueue, &dummy, 0) == pdTRUE);

    Serial.println("START button pressed - Question Started");

    beep(3);

    // Set game state flags
    gameStarted = true;
    winnerDeclared = false;

    // STEP 2: Send GO only after START button
    sendPacket(GO, 0);
    Serial.println("GO Sent to all players");

    // Wait for player response
    xQueueReceive(playerQueue, &player, portMAX_DELAY);

    winnerID = player;
    winnerDeclared = true;
    gameStarted = false;

    Serial.print("Winner Declared: Player ");
    Serial.println(winnerID);

    beep(1);

    // STEP 3: Send LOCK to winner
    sendPacket(LOCK, winnerID);
    Serial.println("LOCK Sent to winner");

    // STEP 4: Wait for NEXT button to reset
    Serial.println("Waiting for NEXT button...");
    xSemaphoreTake(nextSemaphore, portMAX_DELAY);

    // STEP 5: Send RESET only after NEXT button
    sendPacket(RESET, 0);
    Serial.println("RESET Sent - All players reset");
    
    // Loop back to STEP 1 - wait for START again
  }
}

//----------------------------------------------------
void addPeer(uint8_t *mac)
{
  esp_now_peer_info_t peerInfo = {};

  memcpy(peerInfo.peer_addr, mac, 6);

  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) == ESP_OK)
  {
    Serial.println("Peer Added");
  }
  else
  {
    Serial.println("Failed to Add Peer");
  }
}

//----------------------------------------------------
void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  pinMode(START_BTN, INPUT_PULLUP);
  pinMode(NEXT_BTN, INPUT_PULLUP);

  attachInterrupt(START_BTN, startISR, FALLING);
  attachInterrupt(NEXT_BTN, nextISR, FALLING);

  startSemaphore = xSemaphoreCreateBinary();
  nextSemaphore = xSemaphoreCreateBinary();

  playerQueue = xQueueCreate(6, sizeof(uint8_t));

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW Init Failed");
    while (1);
  }

  addPeer(player1);
  addPeer(player2);
  addPeer(player3);
  addPeer(player4);
  addPeer(player5);
  addPeer(player6);

  esp_now_register_recv_cb(OnDataRecv);

  xTaskCreate(
      QuizTask,
      "QuizTask",
      4096,
      NULL,
      2,
      NULL);
}

//----------------------------------------------------
void loop()
{
}