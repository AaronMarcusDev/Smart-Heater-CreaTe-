// Currently not in use
class Receiver {
public:
    void begin(int txPin, int rxPin, unsigned long baud = 115200) {
        Serial1.begin(baud, SERIAL_8N1, rxPin, txPin);
    }

    void update() {
        if (Serial1.available()) {
            String msg = Serial1.readStringUntil('\n');
            Serial.println("Received: " + msg);
        }
    }
};
