// Install DHT Sensor library
// https://github.com/adafruit/DHT-sensor-library
#include <DHT.h>
#include <DHT_U.h>
#include "thermometer.h"
#include <pins_arduino.h>

#define DHTTYPE DHT11
#define DHTPIN  D1

DHT dht(DHTPIN, DHTTYPE);

void setupThermometer() {
  dht.begin();
}


// isnan(temperature) means read error
float getTemperature() {
  float temperature = dht.readTemperature();
  
  if (isnan(temperature)) {
    Serial.println(F("Error reading temperature!"));
  } else {
    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C"));
  }

  return temperature;
}

float getHumidity() {

  float humidity = dht.readHumidity();
  
  if (isnan(humidity)) {
    Serial.println(F("Error reading humidity!"));
  } else {
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.println(F("%"));
  }

  return humidity;
}
