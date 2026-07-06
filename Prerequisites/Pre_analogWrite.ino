const int pwmPin = 4;


// void fade(){
//   for(int i=0 ;i<=255;i++){
//     ledcWrite(pwmPin, i);
//     delay(5); // 50% duty cycle (0-255 for 8-bit)
//   }
//     for(int i=255 ;i>=0;i--){
//     ledcWrite(pwmPin, i);
//     delay(5); // 50% duty cycle (0-255 for 8-bit)
//   }
// }
void setup() {
  // Attach GPIO 4 to PWM
  ledcAttach(pwmPin, 5000, 8); // 5 kHz frequency, 8-bit resolution
}

void loop() {
  ledcWrite(pwmPin, 0); // 50% duty cycle (0-255 for 8-bit)
  // fade();
}
