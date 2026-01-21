// class Receiver {
// public:
//     void begin(int txPin, int rxPin, unsigned long baud = 115200) {
//         Serial1.begin(baud, SERIAL_8N1, rxPin, txPin);
//     }

//     void update() {
//         if (Serial1.available()) {
//             String msg = Serial1.readStringUntil('\n');
//             Serial.println("Received: " + msg);
//         }
//     }
// };

#include <Servo.h>

class Receiver {
public:
    void begin(int txPin, int rxPin, int servoPin, unsigned long baud = 115200) {
        Serial1.begin(baud, SERIAL_8N1, rxPin, txPin);
        servo.attach(servoPin);
    }

    void update() {
        if (Serial1.available()) {
            String msg = Serial1.readStringUntil('\n');
            msg.trim();

            if (msg == "START") {
                servo.write(180);   // full speed forward
                Serial.println("Servo: FORWARD");
            }
            else if (msg == "STOP") {
                servo.write(90);    // stop (neutral)
                Serial.println("Servo: STOP");
            }
            else if (msg == "REVERSE") {
                servo.write(0);     // full speed reverse
                Serial.println("Servo: REVERSE");
            }
            else {
                Serial.println("Unknown command: " + msg);
            }
        }
    }

private:
    Servo servo;
};



// Receiver rx;

// void setup() {
//     Serial.begin(115200);     // USB serial for printing
//     rx.begin(17, 16);         // UART pins for Serial1
// }

// void loop() {
//     rx.update();              // keep checking for messages
// }

Receiver rx;

void setup() {
    Serial.begin(115200);

    // XIAO ESP32S3 pins:
    // TX = 17, RX = 16 (example)
    // Servo on pin D1 (GPIO1)
    rx.begin(17, 16, 1);
}

void loop() {
    rx.update();
}
