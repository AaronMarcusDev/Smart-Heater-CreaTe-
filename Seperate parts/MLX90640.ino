#include <Wire.h>
#include <Adafruit_MLX90640.h>

Adafruit_MLX90640 mlx;
float frame[32 * 24];

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  if (!mlx.begin()) {
    Serial.println("MLX90640 not detected!");
    while (1);
  }

  Serial.println("MLX90640 detected");

  mlx.setMode(MLX90640_CHESS);
  mlx.setResolution(MLX90640_ADC_18BIT);
  mlx.setRefreshRate(MLX90640_8_HZ);
}

void loop() {
  if (mlx.getFrame(frame) != 0) {
    Serial.println("Failed to read frame");
    return;
  }

  Serial.print("Pixel[0]: ");
  Serial.println(frame[0]);
  Serial.print("Pixel[400]: ");
  Serial.println(frame[400]);
  Serial.print("Pixel[767]: ");
  Serial.println(frame[767]);

  Serial.println("--------------------");
  delay(500);
}
