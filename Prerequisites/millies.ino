#define LED_PIN 25
int current_time = millis();
bool State = 0;
int number=0;
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

//millies() code
// void loop() {
//     if(millis() - current_time >= 1000){
//       current_time = millis();
//       State = !State;
//       digitalWrite(LED_PIN, State);
//       Serial.println("LED Toggled!");
//     }
//     number++;
//     Serial.println(number);
//     delay(100);
//      if(number == 10)
//      {number=0;}
  
// }


// Delay code
// void loop(){
//   State = !State;
//       digitalWrite(LED_PIN, State);
//       Serial.println("LED Toggled!");
//       delay(1000);
//        number++;
//       Serial.println(number);
//       delay(100);
//      if(number == 10)
//      {number=0;}
// }