#include <PubSubClient.h>
#include <WiFi.h>
#include <Arduino.h>

void mqtt_send_lamp_status();
void mqtt_send_temphum();
void connection_setup();
void connection_loop();
void callback(char* topic, byte* payload, unsigned int length);

extern const char* ssid;
extern const char* password;
extern const char* mqttServer;
extern const int mqttPort;
extern const char* mqttUser;
extern const char* mqttPassword;
extern const char* mqttTopic_light;
extern const char* mqttTopic_temperature;
extern const char* mqttTopic_humidity;
extern const char* mqttTopic_heater;

#define LED 13
#define LED0 32
#define LED_ON HIGH
#define LED_OFF LOW

extern int temp_set;

void mqtt_send_heater_status();
void mqtt_lamp_on();
void mqtt_lamp_off();