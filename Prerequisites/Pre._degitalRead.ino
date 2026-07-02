#define LED_PIN 25

void setup() {
    Serial.begin(115200);
  pinMode(LED_PIN, INPUT_PULLUP);
}
void loop() {
   bool buttonState = digitalRead(LED_PIN);
    if (buttonState == LOW) {
        Serial.println("Button Pressed");
    } else {
        Serial.println("Button Released");
    }
}