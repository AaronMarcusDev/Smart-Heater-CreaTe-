#include <Wire.h>
#include <Adafruit_MLX90640.h>
#include <ESP32Servo.h>

/* =========================
   PIN DEFINITIONS
   ========================= */
#define SERVO_PIN   18
#define MOSFET_PIN  17
#define SDA_PIN     21
#define SCL_PIN     22

/* =========================
   CAMERA GRID CLASS
   ========================= */
class CameraGrid {
public:
  static const int WIDTH  = 32;
  static const int HEIGHT = 24;

  bool begin() {
    Serial.println("[Camera] Initializing MLX90640...");
    if (!mlx.begin()) {
      Serial.println("[Camera] ❌ MLX90640 not detected");
      return false;
    }

    mlx.setMode(MLX90640_CHESS);
    mlx.setResolution(MLX90640_ADC_18BIT);
    mlx.setRefreshRate(MLX90640_4_HZ);

    Serial.println("[Camera] ✅ MLX90640 ready");
    return true;
  }

  bool readFrame() {
    int status = mlx.getFrame(frame);
    return (status == 0);
  }

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

  int indexToX(int index) {
    return index % WIDTH;
  }

private:
  Adafruit_MLX90640 mlx;
  float frame[WIDTH * HEIGHT];
};

/* =========================
   TARGET MAPPER CLASS
   ========================= */
class TargetMapper {
public:
  TargetMapper(int minAngle, int maxAngle)
    : minAngle(minAngle), maxAngle(maxAngle) {}

  int gridXToServoAngle(int x) {
    return map(x, 0, 31, minAngle, maxAngle);
  }

private:
  int minAngle;
  int maxAngle;
};

/* =========================
   HEATER + FAN CLASS
   ========================= */
class HeaterSystem {
public:
  void begin() {
    Serial.println("[Heater] Initializing servo and MOSFET");

    servo.setPeriodHertz(50);
    servo.attach(SERVO_PIN, 500, 2400);

    pinMode(MOSFET_PIN, OUTPUT);
    digitalWrite(MOSFET_PIN, LOW);

    Serial.println("[Heater] ✅ Ready (heater OFF)");
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

/* =========================
   GLOBAL OBJECTS
   ========================= */
CameraGrid camera;
TargetMapper mapper(30, 150);   // safe servo limits
HeaterSystem heater;

/* =========================
   SETUP
   ========================= */
void setup() {
  Serial.begin(115200);
  delay(1000); // allow serial to settle

  Serial.println("\n=== SMART HEATING SYSTEM BOOT ===");

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);  // REQUIRED for MLX90640

  Serial.println("[System] I2C initialized");

  bool cameraOK = camera.begin();
  if (!cameraOK) {
    Serial.println("[System] ⚠️ Camera failed, continuing without it");
  }

  heater.begin();

  Serial.println("[System] Setup complete");
}

/* =========================
   MAIN LOOP
   ========================= */
void loop() {
  static unsigned long lastToggle = 0;
  static bool heaterState = false;

  /* ---- HEATER TOGGLE (INDEPENDENT) ---- */
  if (millis() - lastToggle > 3000) {
    lastToggle = millis();
    heaterState = !heaterState;

    if (heaterState) {
      heater.heaterOn();
      Serial.println("[Heater] 🔥 ON");
    } else {
      heater.heaterOff();
      Serial.println("[Heater] ❄️ OFF");
    }
  }

  /* ---- CAMERA PROCESSING (NON-BLOCKING) ---- */
  if (camera.readFrame()) {
    int hottestPixel = camera.getHottestPixel();
    int x = camera.indexToX(hottestPixel);
    int angle = mapper.gridXToServoAngle(x);

    heater.setServoAngle(angle);

    Serial.print("[Camera] Hottest X=");
    Serial.print(x);
    Serial.print(" → Servo=");
    Serial.println(angle);
  } else {
    Serial.println("[Camera] ⚠️ Frame read failed");
  }

  delay(200);
}
