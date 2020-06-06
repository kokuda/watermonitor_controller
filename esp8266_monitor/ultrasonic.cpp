#include <Arduino.h>
#include <pins_arduino.h>
#include "NewPing.h"
#include "ultrasonic.h"

const int TriggerPin = D3;
const int EchoPin = D2;
NewPing hcsr04(TriggerPin,EchoPin);

unsigned int getUltrasonicSensorPingTimeUs() {
  unsigned int ping_us = hcsr04.ping(200);
  unsigned int distance = 	(ping_us / US_ROUNDTRIP_CM);

  Serial.print(F("Distance: ")); Serial.print(distance); Serial.println(F("[cm]"));

  return ping_us;
}
