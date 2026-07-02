void setup() {
    Serial.begin(115200);
}
void loop() {

   int Potentionmeter_State = analogRead(33);
    Serial.print("Potentiometer Value: ");
    Serial.println(Potentionmeter_State);
    delay(100);
}