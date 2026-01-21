// This is for the ESP 2 (The TinyC6)

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
    delay(1000);
}
