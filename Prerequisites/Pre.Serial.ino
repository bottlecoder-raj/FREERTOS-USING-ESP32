#define LED_PIN 25

void setup() {
    Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}
void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(1000);
  digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
  delay(1000);
}