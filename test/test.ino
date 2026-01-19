#define MOSFET_PIN 17

void setup() {
  Serial.begin(115200);
  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);
  Serial.println("Setup done");
}

void loop() {
  Serial.println("MOSFET ON");
  digitalWrite(MOSFET_PIN, HIGH);
  delay(2000);

  Serial.println("MOSFET OFF");
  digitalWrite(MOSFET_PIN, LOW);
  delay(2000);
}
