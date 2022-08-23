#include "mydht.h"

DHT dht(DHTPIN, DHTTYPE);

void printDHT(){
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
    }
    // print the result to Terminal
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C ");
}

void dht_init(){
    dht.begin();
}

float getTemp(){
    float t = dht.readTemperature();
    return t;
}

float getHum(){
    float h = dht.readHumidity();
    return h;
}
