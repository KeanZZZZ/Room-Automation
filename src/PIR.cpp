#include "PIR.h"



void PIR_setup() 
{
  Serial.begin(115200);  // Serial is used to view Analog out
  // Analog and digital pins should both be set as inputs:
  pinMode(PIR_AOUT, INPUT);
  pinMode(PIR_DOUT, INPUT);

  // Configure the motion indicator LED pin as an output
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Turn the LED off
}

int readDigitalValue()
{
  // The OpenPIR's digital output is active high
  int motionStatus = digitalRead(PIR_DOUT);
  return motionStatus;
}

float printAnalogValue()
{
    // Read in analog value:
    unsigned int analogPIR = analogRead(PIR_AOUT);
    // Convert 10-bit analog value to a voltage
    // (Assume high voltage is 5.0V.)
    float voltage = (float) analogPIR / 1024.0 * 5.0;
    // Print the reading from the digital pin.
    // Mutliply by 5 to maintain scale with AOUT. 
    Serial.print(5 * digitalRead(PIR_DOUT));
    Serial.print(',');    // Print a comma
    Serial.print(2.5);    // Print the upper limit
    Serial.print(',');    // Print a comma
    Serial.print(1.7);    // Print the lower limit
    Serial.print(',');    // Print a comma
    Serial.print(voltage); // Print voltage
    Serial.println();
    return voltage;
}

