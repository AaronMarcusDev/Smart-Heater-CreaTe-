class HeaterSystem {
public:
  void begin() {
    servo.setPeriodHertz(50);
    servo.attach(SERVO_PIN, 500, 2400);

    pinMode(MOSFET_PIN, OUTPUT);
    digitalWrite(MOSFET_PIN, LOW); // heater off
  }

  void setServoAngle(int angle) {
    servo.write(angle);
  }

  void heaterOn() {
    digitalWrite(MOSFET_PIN, HIGH);
  }

  void heaterOff() {
    digitalWrite(MOSFET_PIN, LOW);
  }

private:
  Servo servo;
};