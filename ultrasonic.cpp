#include <Arduino.h>
#include <pins_arduino.h>
#include "NewPing.h"
#include "ultrasonic.h"

const int TriggerPin = D2;
const int EchoPin = D1;
NewPing hcsr04(TriggerPin,EchoPin);

float getDistance(unsigned int ping_us, float temperature) {
  float distance = (331.5f + 0.6f * temperature) * float(ping_us) / 20000.0f;
  return distance;
}

unsigned int getUltrasonicSensorPingTimeUs() {
  unsigned int ping_us = hcsr04.ping(200);

  Serial.print(F("Ping: ")); Serial.print(ping_us); Serial.println(F("[microseconds]"));

  return ping_us;
}
