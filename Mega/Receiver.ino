#include <WiFi.h>
#include <esp_now.h>

//---------------- Pins ----------------
#define BUTTON_PIN   18
#define LED_PIN      19
#define BUZZER_PIN   23

// Change this for every board
#define PLAYER_ID    1

// Master's MAC Address
uint8_t masterMAC[] = {0x24,0x6F,0x28,0xAA,0xBB,0xCC};

//--------------------------------------

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

//--------------------------------------

bool gameEnabled = false;
bool iAmWinner = false;
bool alreadyBuzzed = false;

//--------------------------------------

void blinkTask(void *pvParameters)
{
    while (1)
    {
        if (iAmWinner)
        {
            digitalWrite(LED_PIN, HIGH);
            vTaskDelay(pdMS_TO_TICKS(300));

            digitalWrite(LED_PIN, LOW);
            vTaskDelay(pdMS_TO_TICKS(300));
        }
        else
        {
            digitalWrite(LED_PIN, LOW);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

//--------------------------------------

void IRAM_ATTR buttonISR()
{
    if (!gameEnabled || alreadyBuzzed)
        return;

    alreadyBuzzed = true;

    txPacket.type = BUZZ;
    txPacket.playerID = PLAYER_ID;

    esp_now_send(masterMAC, (uint8_t *)&txPacket, sizeof(txPacket));
}

//--------------------------------------

void OnDataRecv(const esp_now_recv_info_t *info,
                const uint8_t *incomingData,
                int len)
{
    memcpy(&rxPacket, incomingData, sizeof(rxPacket));

    switch (rxPacket.type)
    {

    case GO:

        gameEnabled = true;
        alreadyBuzzed = false;
        iAmWinner = false;

        Serial.println("GO");
        break;

    case LOCK:

        gameEnabled = false;

        if (rxPacket.playerID == PLAYER_ID)
        {
            iAmWinner = true;

            digitalWrite(BUZZER_PIN, HIGH);
            delay(150);
            digitalWrite(BUZZER_PIN, LOW);

            Serial.println("I WON");
        }

        break;

    case RESET:

        gameEnabled = false;
        alreadyBuzzed = false;
        iAmWinner = false;

        digitalWrite(LED_PIN, LOW);

        Serial.println("RESET");

        break;
    }
}

//--------------------------------------

void setup()
{
    Serial.begin(115200);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    attachInterrupt(BUTTON_PIN,
                    buttonISR,
                    FALLING);

    WiFi.mode(WIFI_STA);

    esp_now_init();

    esp_now_register_recv_cb(OnDataRecv);

    esp_now_peer_info_t peerInfo = {};

    memcpy(peerInfo.peer_addr,
           masterMAC,
           6);

    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    esp_now_add_peer(&peerInfo);

    xTaskCreate(
        blinkTask,
        "Blink",
        2048,
        NULL,
        1,
        NULL);
}

void loop()
{
}