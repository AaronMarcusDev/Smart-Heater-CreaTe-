//! THIS IS ALSO IN THE FINAL MAIN INO FILE BUT IS NOT LISTED ON GITHUB!!!

#include <Wire.h>

// Just a simple sensor data --> human interpretable data converter :D
String getCOLevel(int adcValue)
{
    if (adcValue < 1200)
    {
        return "LOW";
    }
    else if (adcValue < 2500)
    {
        return "MEDIUM";
    }
    else
    {
        return "HIGH";
    }
}

// This is to get a more consistent reading, the changes can be quite volatile, so this should help.
int smoothRead(int pin)
{
    long sum = 0;
    for (int i = 0; i < 10; i++)
    {
        sum += analogRead(pin);
        delay(5);
    }
    return sum / 10;
}

// I might as well throw in a function for sending the CO data.
void sendCO(int value, String humanReadableValue){
  Serial.print("TX-CO:");
  Serial.println(value);
  Serial.print("TX-COH:");
  Serial.println(humanReadableValue);
} 

void setup()
{
    Serial.begin(115200);
    delay(500);
    
    Serial.println("Started reading CO.");

    int raw = analogRead(4); // if AO is connected to GPIO4
    Serial.print("Initial read: ");
    Serial.println(raw);     // I always print the first reading just for testing.
}

void loop()
{
    int raw = smoothRead(4);
    Serial.print("The raw data is: ");
    Serial.println(raw);
    Serial.print("That means that the level of CO is: ");
    Serial.println(getCOLevel(raw));

    // Sending to main ESP32
    sendCO(raw, getCOLevel(raw));

    delay(1000);
}

// To read it:
// String incoming = "";
// int coValue = 0;
// String coHuman = "";

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Receiver ready.");
// }

// void loop() {
//   if (Serial.available()) {
//     incoming = Serial.readStringUntil('\n');
//     incoming.trim();

//     // Check for numeric CO value
//     if (incoming.startsWith("TX-CO:")) {
//       String numberPart = incoming.substring(6);
//       coValue = numberPart.toInt();
//       Serial.print("Received CO value: ");
//       Serial.println(coValue);
//     }

//     // Check for human-readable CO level
//     else if (incoming.startsWith("TX-COH:")) {
//       coHuman = incoming.substring(7);
//       Serial.print("Received CO level: ");
//       Serial.println(coHuman);
//     }
//   }
// }

