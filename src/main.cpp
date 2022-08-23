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
void detect_distance(){
  int det = readDigitalValue();
  if (det == HIGH && light_isClose && !trigger)
  {
      tonSig2();
      mqtt_lamp_on();
      digitalWrite(LED0, HIGH);
      trigger = true;
  }
  else if(det == LOW && light_isClose && trigger){
      light_isClose = false;
      trigger = false;
  }
  else if(det == HIGH && !light_isClose && !trigger)
  {
      toffSig2();
      mqtt_lamp_off();
      digitalWrite(LED0, LOW);
      trigger = true;
  }
  else if(det == LOW && !light_isClose && trigger){
      light_isClose = true;
      trigger = false;
  }
}

void auto_temp(){
  float temp = getTemp();
  if(temp<temp_set && isClose){
    tonSig1();
    //digitalWrite(LED, HIGH);
    isClose = !isClose;
  }
  else if(temp>temp_set && !isClose){
    toffSig1();
    //digitalWrite(LED, LOW);
    isClose = !isClose;
  }
}
 
void loop(void) 
{ 
  simpleRead(); //220 to open the light

  printDHT();

  measure_usonic();

  connection_loop();

  detect_distance();

  int con = digitalRead(LED);
  if(con == HIGH){
    auto_temp();
  }else if(con == LOW && !isClose){
    toffSig1();
    isClose = true;
  }

  if(++loopslice==100){
    mqtt_send_temphum();
    loopslice=0;
  }
  delay(500);
}