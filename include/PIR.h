#include <Arduino.h>

//////////////////////////
// Hardware Definitions //
//////////////////////////
#define PIR_AOUT 36  // PIR analog output on 36
#define PIR_DOUT 39   // PIR digital output on 39
#define LED_PIN  32  // LED to illuminate on motion

void PIR_setup();
int readDigitalValue();
float printAnalogValue();