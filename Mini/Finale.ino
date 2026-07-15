#include <Arduino.h>

// ---------------- Pins ----------------
#define IR_PIN      25
#define POT_PIN     34
#define RED_LED     18
#define GREEN_LED   19
#define BUZZER      23

// --------------- FreeRTOS Objects ---------------
QueueHandle_t potQueue;
SemaphoreHandle_t irSemaphore;
TimerHandle_t alarmTimer;

// Alarm duration (ms)
int alarmDuration = 2000;

//--------------------------------------------------
// Timer Callback
//--------------------------------------------------
void timerCallback(TimerHandle_t xTimer)
{
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(BUZZER, LOW);

    Serial.println("Alarm OFF");
}

//--------------------------------------------------
// ISR
//--------------------------------------------------
void IRAM_ATTR irISR()
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(irSemaphore, &higherPriorityTaskWoken);

    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

//--------------------------------------------------
// Potentiometer Task
//--------------------------------------------------
void PotTask(void *pvParameters)
{
    int value;

    while (1)
    {
        value = analogRead(POT_PIN);

        // Convert ADC to 1000-5000 ms
        int duration = map(value, 0, 4095, 1000, 5000);

        xQueueOverwrite(potQueue, &duration);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

//--------------------------------------------------
// Alarm Task
//--------------------------------------------------
void AlarmTask(void *pvParameters)
{
    int duration;

    while (1)
    {
        if (xSemaphoreTake(irSemaphore, portMAX_DELAY) == pdTRUE)
        {
            if (xQueueReceive(potQueue, &duration, 0) == pdTRUE)
            {
                alarmDuration = duration;
            }

            Serial.print("Intruder! Alarm Duration: ");
            Serial.println(alarmDuration);

            digitalWrite(GREEN_LED, LOW);
            digitalWrite(RED_LED, HIGH);
            digitalWrite(BUZZER, HIGH);

            // Restart timer with new duration
            xTimerChangePeriod(alarmTimer,
                               pdMS_TO_TICKS(alarmDuration),
                               0);

            xTimerStart(alarmTimer, 0);
        }
    }
}

//--------------------------------------------------
void setup()
{
    Serial.begin(115200);

    pinMode(IR_PIN, INPUT_PULLUP);

    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    digitalWrite(GREEN_LED, HIGH);

    // Queue stores one integer
    potQueue = xQueueCreate(1, sizeof(int));

    // Binary Semaphore
    irSemaphore = xSemaphoreCreateBinary();

    // Software Timer
    alarmTimer = xTimerCreate(
        "Alarm Timer",
        pdMS_TO_TICKS(2000),
        pdFALSE,
        NULL,
        timerCallback);

    // Tasks
    xTaskCreate(
        PotTask,
        "Pot Task",
        2048,
        NULL,
        1,
        NULL);

    xTaskCreate(
        AlarmTask,
        "Alarm Task",
        2048,
        NULL,
        2,
        NULL);

    attachInterrupt(
        digitalPinToInterrupt(IR_PIN),
        irISR,
        FALLING);
}

void loop()
{
}