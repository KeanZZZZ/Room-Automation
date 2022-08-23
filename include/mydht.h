#include <DHT.h>
#include <Arduino.h>

extern DHT dht;
#define DHTPIN 18
#define DHTTYPE DHT22

void printDHT();
void dht_init();
float getTemp();
float getHum();