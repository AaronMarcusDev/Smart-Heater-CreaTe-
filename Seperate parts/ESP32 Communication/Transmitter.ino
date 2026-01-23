// Currently not in use
class Transmitter
{
public:
    void begin(int txPin, int rxPin, unsigned long baud = 115200)
    //                                                      ^-- Make sure this lines up in the reciever ESP32!!!
    {
        Serial1.begin(baud, SERIAL_8N1, rxPin, txPin); // Should be pin TX0 ??
        //                          ^-- 8 bits, an N bit and a Stop bit.
    }

    void send(const String &msg)
    //                      ^-- Like a message pointerf
    {
        Serial1.println(msg);
    }

    void start() { send("START"); }
    void stop() { send("STOP"); }
};

// Example usage:
// Transmitter tx;
// void setup()
// {
//     tx.begin(17, 16); // TX=17, RX=16
// }
// void loop()
// {
//     tx.start();
//     delay(2000);
//     tx.stop();
//     delay(2000);
// }