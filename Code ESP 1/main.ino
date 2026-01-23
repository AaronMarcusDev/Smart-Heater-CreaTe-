// TEAM MEMBERS READ!!
// This is all of the code that is currntly running on the ESP32-C DevKit (Our main board),
// However, also the CO Meter is incorporated, but we merged them on the spot, so you have to read that as well to fully understand what is ACTUALLY RUNNING.
// It can be found in ~/Seperate parts/CO-Meter.ino

// Libraries
#include <Wire.h>
#include <Adafruit_MLX90640.h>
#include <ESP32Servo.h>

// Constants / Pins
#define SERVO_PIN   18
#define MOSFET_PIN  17
#define SDA_PIN     21 // Check the colours on the breadboard!!
#define SCL_PIN     22 // ^^

// Camera Grid
class CameraGrid {
public:
  static const int WIDTH  = 32;
  static const int HEIGHT = 24;

  bool begin() {
    Serial.println("[Camera] Initializing MLX90640...");
    if (!mlx.begin()) {
      Serial.println("[Camera] MLX90640 not detected");
      return false;
    }

    mlx.setMode(MLX90640_CHESS);
    mlx.setResolution(MLX90640_ADC_18BIT);
    mlx.setRefreshRate(MLX90640_4_HZ); // Four times per second it outputs a frame

    Serial.println("[Camera] MLX90640 ready");
    return true;
  }

  // A 'remapping' of the mlx.getFrame(...) to make it integrated in the CameraGrid class
  bool readFrame() {
    int status = mlx.getFrame(frame);
    return (status == 0);
  }

  // Used for, well, determining the hottest pixel, that we can use for the actual tracking
  int getHottestPixel() {
    int hottestIndex = 0;
    float maxTemp = frame[0];

    for (int i = 1; i < WIDTH * HEIGHT; i++) {
      //                    ^-- So the full grid
      if (frame[i] > maxTemp) {
        maxTemp = frame[i];
        hottestIndex = i;
      }
      // Just a simple loop that saves the (current) highest value
      // --> The hottest place (from above) :)
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

// Target Mapper
class TargetMapper {
public:
// constructor
  TargetMapper(int minAngle, int maxAngle) : minAngle(minAngle), maxAngle(maxAngle) {}

  int gridXToServoAngle(int x) {
    return map(x, 0, 31, minAngle, maxAngle);
    // using the servo range and mapping to it from our X coordinate
  }

private:
  int minAngle;
  int maxAngle;
};

// Heater & Fan System
class HeaterSystem {
public:
  void begin() {
  Serial.println("[Heater] Initializing servo and MOSFET");

  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 500, 2400);

  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);

  servo.write(90);   // So it starts centered
  delay(500);        // give it time to let it physically move

  Serial.println("[Heater] Ready (centered, heater ON)");
}


  void setServoAngle(int angle) {
    servo.write(angle);
  }

  // These should explain themselves (:
  void heaterOn() {
    digitalWrite(MOSFET_PIN, HIGH);
  }

  void heaterOff() {
    digitalWrite(MOSFET_PIN, LOW);
  }

private:
  Servo servo;
};

// Global Objects
CameraGrid camera;
TargetMapper mapper(30, 150);   // safe servo limits (In our case),
HeaterSystem heater;            // ^-- If it doesnt turn far enough to the right, increase the 150 --> 160

// main (setup)
void setup() {
  Serial.begin(115200);
  delay(1000); // allow serial to settle

  Serial.println("\nHeating system starting up");

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);  // REQUIRED for MLX90640

  Serial.println("[System] I2C initialized");

  bool cameraOK = camera.begin();
  if (!cameraOK) {
    Serial.println("[System] !!! Camera failed, continuing without it !!!");
  }

  heater.begin();

  Serial.println("[System] Setup complete");
}

// Runtime / loop
void loop() {
  static unsigned long lastToggle = 0;
  static bool heaterState = false;

  /* ---- HEATER TOGGLE (INDEPENDENT) ---- */
  // if (millis() - lastToggle > 3000) {
  //   lastToggle = millis();
  //   heaterState = !heaterState;

  //   if (heaterState) {
  //     heater.heaterOn();
  //     Serial.println("[Heater] ON");
  //   } else {
  //     heater.heaterOff();
  //     Serial.println("[Heater] OFF");
  //   }

    heater.heaterOn(); // Lets just keep it on for now, it does stop at first in the setup(),
    //                    But it might be worth the time saving of writing a controller for it.
    //                    Otherwise just uncomment the code above and remove this line.

  // Camera Data Processing
  if (camera.readFrame()) {
    int hottestPixel = camera.getHottestPixel();
    int x = camera.indexToX(hottestPixel);
    int angle = mapper.gridXToServoAngle(x);

    heater.setServoAngle(angle);

    Serial.print("[Camera] Hottest X=");
    Serial.print(x);
    Serial.print(" --> Servo=");
    Serial.println(angle);
  } else {
    Serial.println("[Camera] !!! Frame read failed !!!");
  }

  delay(200);
}