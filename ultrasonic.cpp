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


///////////////////////////////////////////////////////////////////////////////

const float maxCmPerMs = 1.0f / 1000.0f;

float validateDistance(float distance, unsigned long nowMs) {
  static unsigned long lastTimeMs = 0;
  static float lastDistance = 0;

  if (lastTimeMs != 0) {
    unsigned long deltaMs = nowMs - lastTimeMs;
    float maxDeltaDistance = maxCmPerMs * static_cast<float>(deltaMs);
    float deltaDistance = abs(distance - lastDistance);
    if (deltaDistance > maxDeltaDistance) {
      // Ignore impossible distances
      return lastDistance;
    }
  }

  lastTimeMs = nowMs;
  lastDistance = distance;
  return distance;
}
