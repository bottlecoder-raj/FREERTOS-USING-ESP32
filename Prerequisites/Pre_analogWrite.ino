const int pwmPin = 25;

void setup() {
  // Attach GPIO 4 to PWM
  ledcAttach(pwmPin, 5000, 8); // 5 kHz frequency, 8-bit resolution
}

void loop() {
  ledcWrite(pwmPin, 128); // 50% duty cycle (0-255 for 8-bit)
}