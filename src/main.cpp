#include <Arduino.h>
#include "test_tsl.h"
#include "mydht.h"
#include "RFcontrol.h"
#include "ultrasonic.h"
#include "io_connect.h"
#include "PIR.h"

int loopslice = 0;
bool isClose = true;
bool auto_temp_con = false;
bool light_isClose = true;
int temp_set = 15;
 

void setup(void) 
{
  Serial.begin(115200);
 
  Serial.println("Starting Adafruit TSL2591 Test!");

  init_usonic();

  initial_RF();// init for remote control sokcet

  dht_init();
 
  printMessage();
 
  /* Configure the sensor */
  configureSensor();

  connection_setup();

  PIR_setup();
}

bool trigger = false;
bool should_light = false;
void detect_motion(){
  int det = readDigitalValue();
  int brightness = simpleRead();
  if (det == HIGH && light_isClose && !trigger)//Entering the room
  {
      if(brightness<1000){
        tonSig2();
        tonSig2();  //In case transport fail
        mqtt_lamp_on();
        should_light = true;
        digitalWrite(LED0, HIGH);
      }else should_light = false;
      trigger = true;
  }
  else if(det == LOW && light_isClose && trigger){//already enterd the room
      light_isClose = false; 
      trigger = false;
  }
  else if(det == LOW && !light_isClose && !trigger){//keep detecting the brightness
      if(brightness<1000 && digitalRead(LED0) == LOW){  ///when people entered
        tonSig2();
        tonSig2();
        mqtt_lamp_on();
        digitalWrite(LED0, HIGH);
        Serial.println("dark");
      }else if(brightness>=1000 && digitalRead(LED0) == HIGH){
        toffSig2();
        toffSig2();
        mqtt_lamp_off();
        digitalWrite(LED0, LOW);
        Serial.println("light");
      }
  }
  else if(det == HIGH && !light_isClose && !trigger)//leaving the room
  {
      toffSig2();
      toffSig2();
      mqtt_lamp_off();
      digitalWrite(LED0, LOW);
      trigger = true;
  }
  else if(det == LOW && !light_isClose && trigger){//already left the room
      light_isClose = true;
      trigger = false;
  }
}

void auto_temp(){
  float temp = getTemp();
  if(temp<temp_set && isClose){
    tonSig1();  //A434 transmits the signal
    tonSig1();  //In case the transmission message corrupt
    isClose = !isClose;
  }
  else if(temp>temp_set && !isClose){
    toffSig1();
    toffSig1();
    isClose = !isClose;
  }
}
 
void loop(void) 
{ 
  //simpleRead(); //800 to open the light
  printAnalogValue();

  printDHT();

  measure_usonic();

  connection_loop();

  if(flag_lamp) detect_motion();
  else{
    int light_con = digitalRead(LED0);
    if(light_con==HIGH){
      tonSig2();
      Serial.println("Light open manully");
    }else toffSig2();
  }

  int con = digitalRead(LED);
  if(con == HIGH){
    auto_temp();
  }else if(con == LOW && !isClose){
    toffSig1();
    toffSig1();
    isClose = true;
  }

  if(++loopslice==100){
    mqtt_send_temphum();
    loopslice=0;
  }
  delay(500);
}