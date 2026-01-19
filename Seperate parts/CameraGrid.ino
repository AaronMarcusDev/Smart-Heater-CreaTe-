lass CameraGrid {
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