#include <ESP32Servo.h>

Servo heaterServo;
int servoPin = 18;

void setup() {
  heaterServo.setPeriodHertz(50);
  heaterServo.attach(servoPin, 500, 2400);
}

void loop() {
  int targetAngle = 90; // computed from MLX90640
  heaterServo.write(targetAngle);
  delay(500);
}
