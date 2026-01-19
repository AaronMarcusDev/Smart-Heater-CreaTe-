#define SERVO_PIN 18
#define HEATER_PIN 17 // For the MOSFET that controls the heater and fan

void setup() {
    pinMode(HEATER_PIN, OUTPUT);
    digitalWrite(HEATER_PIN, LOW); //start with it turned off

}
void loop() {
    digitalWrite(HEATER_PIN, HIGH); // Turn it on for testing the MOSFET controller
}