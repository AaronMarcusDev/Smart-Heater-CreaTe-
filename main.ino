#include <Wire.h>
#include <Adafruit_MLX90640.h>
#include <ESP32Servo.h>

#define SERVO_PIN   18
#define MOSFET_PIN  17
#define SDA_PIN     21
#define SCL_PIN     22


class CameraGrid {
public:
  static const int WIDTH  = 32;
  static const int HEIGHT = 24;

  float frame[WIDTH * HEIGHT];

  bool begin() {
    if (!mlx.begin()) return false;

    mlx.setMode(MLX90640_CHESS);
    mlx.setResolution(MLX90640_ADC_18BIT);
    mlx.setRefreshRate(MLX90640_8_HZ);
    return true;
  }

  bool readFrame() {
    return (mlx.getFrame(frame) == 0);
  }

  // Finds hottest pixel index
  int getHottestPixel() {
    int hottestIndex = 0;
    float maxTemp = frame[0];

    for (int i = 1; i < WIDTH * HEIGHT; i++) {
      if (frame[i] > maxTemp) {
        maxTemp = frame[i];
        hottestIndex = i;
      }
    }
    return hottestIndex;
  }

  // Convert pixel index to X coordinate (0–31)
  int indexToX(int index) {
    return index % WIDTH;
  }

private:
  Adafruit_MLX90640 mlx;
};


class TargetMapper {
public:
  TargetMapper(int minAngle, int maxAngle)
    : minAngle(minAngle), maxAngle(maxAngle) {}

  int gridXToServoAngle(int x) {
    // Map 0–31 → servo angle range
    return map(x, 0, 31, minAngle, maxAngle);
  }

private:
  int minAngle;
  int maxAngle;
};

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

// Global objects
CameraGrid camera;
TargetMapper mapper(30, 150);   // servo angle limits
HeaterSystem heater;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!camera.begin()) {
    Serial.println("MLX90640 not detected!");
    while (1);
  }

  heater.begin();

  Serial.println("System initialized");
}

void loop() {
  if (!camera.readFrame()) {
    Serial.println("Failed to read thermal frame");
    return;
  }

  int hottestPixel = camera.getHottestPixel();
  int x = camera.indexToX(hottestPixel);
  int servoAngle = mapper.gridXToServoAngle(x);

  heater.setServoAngle(servoAngle);
  heater.heaterOn();

  Serial.print("Hottest X: ");
  Serial.print(x);
  Serial.print(" → Servo angle: ");
  Serial.println(servoAngle);

  delay(500);
}
