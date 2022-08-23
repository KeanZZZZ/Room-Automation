#include "ultrasonic.h"

void init_usonic()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

long measure_usonic()
{
    long duration, distance;
    digitalWrite(trigPin, LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
    delayMicroseconds(10); // Added this line
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1;
    if (distance >= 200 || distance <= 0){
        Serial.println("Out of range");
    }
    else {
        Serial.print(distance);
        Serial.println(" cm");
    }
    return distance;
}
