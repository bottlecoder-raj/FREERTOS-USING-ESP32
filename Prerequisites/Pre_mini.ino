const int RedPin = 2;
const int GreenPin = 4;
const int BluePin = 15;
#define SW 25

int colour_State = 1; //1=Red, 2=Green, 3=Blue

void setup() {
    Serial.begin(115200);
      ledcAttach(RedPin, 5000, 8); // 5 kHz frequency, 8-bit resolution
      ledcAttach(GreenPin, 5000, 8); // 5 kHz frequency, 8-bit resolution
      ledcAttach(BluePin, 5000, 8); // 5 kHz frequency, 8-bit resolution
      pinMode(SW, INPUT_PULLUP);
}
void loop() {
    int Potentiometer_State = analogRead(33);
      // Convert 0-4095 to 0-255
    Potentiometer_State = map(Potentiometer_State, 0, 4095, 0, 255);
    Potentiometer_State=255-Potentiometer_State;
    bool buttonState = digitalRead(SW);
    if (buttonState == LOW) {
        Serial.println("Colour State Changed");
        colour_State = (colour_State % 3) + 1; // Cycle through colours
        delay(100);
    } 
    
    switch (colour_State) {
        case 1:
            ledcWrite(RedPin, Potentiometer_State); // Red ON
            break;
        case 2:
            ledcWrite(GreenPin, Potentiometer_State);   // Green OFF
            break;
        case 3:
            ledcWrite(BluePin, Potentiometer_State);    // Blue ON
            break;

        default:
            break;
    }
    Serial.print("Colour State: ");
    Serial.print(colour_State);
    Serial.print("\t Potentiometer Value: ");
    Serial.println(Potentiometer_State);
    delay(100);
}