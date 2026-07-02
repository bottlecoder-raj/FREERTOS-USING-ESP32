
#define LED1_PIN 25
#define LED2_PIN 26

TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;

void Task1(void *parameter) {
  pinMode(LED1_PIN, OUTPUT);
  for (;;) {
    digitalWrite(LED1_PIN, HIGH);
    Serial.println("Task1: LED1 ON");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(LED1_PIN, LOW);
    Serial.println("Task1: LED1 OFF");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task2(void *parameter) {
  pinMode(LED2_PIN, OUTPUT);
  for (;;) {
    digitalWrite(LED2_PIN, HIGH);
    Serial.println("Task2: LED2 ON");
    vTaskDelay(333 / portTICK_PERIOD_MS);
    digitalWrite(LED2_PIN, LOW);
    Serial.println("Task2: LED2 OFF");
    vTaskDelay(333 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    Task1,             // Task function
    "Task1",           // Task name
    10000,             // Stack size (bytes)
    NULL,              // Parameters
    1,                 // Priority
    &Task1Handle,      // Task handle
    1                  // Core 1
  );

  xTaskCreatePinnedToCore(
    Task2,            // Task function
    "Task2",          // Task name
    10000,            // Stack size (bytes)
    NULL,             // Parameters     
    1,                // Priority
    &Task2Handle,     // Task handle
    1                 // Core 1
  );
}

void loop() {
  // Empty because FreeRTOS scheduler runs the task
}