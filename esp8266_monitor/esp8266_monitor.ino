/**
   esp8266_monitor.ino
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include "config.h"
#include "thermometer.h"
#include "ultrasonic.h"
#include "display.h"
#include "postToAbode.h"
#include "adafruitMqtt.h"

ESP8266WiFiMulti WiFiMulti;

#define LINE_WIFI 0
#define LINE_TEMP 1
#define LINE_DIST 2

void connectWifi(const char* wifi_ssid, const char* wifi_passphrase) {

  Serial.print(F("[SETUP]Connect Wifi: "));
  Serial.print(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(wifi_ssid, wifi_passphrase);

  int retries = 10;
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));

    // Die if this takes too long.
    if (retries-- <= 0) {
      Serial.println(F("[SETUP]Error connecting to Wifi"));
      while(1);
    }
  }

  Serial.println();
  Serial.println(F("[SETUP]Connected to Wifi"));

}

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  if (!setupConfigFile()) {
    return;
  }

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  Serial.println("[SETUP]Loading config");

  // Use ..\esp8266_config to save the config to SPIFFS storage 
  auto config = loadConfigFile();
  if (config.get() == nullptr) {
    Serial.println("Failed to load config file");
    return;
  }

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, config.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return;
  }

  JsonObject configObject = doc.as<JsonObject>();
  const char* wifi_ssid = configObject["wifi_ssid"].as<const char*>();
  const char* wifi_passphrase = configObject["wifi_passphrase"].as<const char*>();

  connectWifi(wifi_ssid, wifi_passphrase);

  setupThermometer();

  initDisplay();

  setDisplayLine(LINE_WIFI, "Wifi: %s", wifi_ssid);
}

void loop() {

  unsigned int ping_us = getUltrasonicSensorPingTimeUs();
  float temperature = getTemperature();
  float humidity = getHumidity();

  float distance = getDistance(ping_us, temperature);
  Serial.print(F("Distance: ")); Serial.print(distance); Serial.println(F("[cm]"));

  postToAbode(ping_us, temperature, humidity);

  if (adafruitMqttConnect()) {
    adafruitMqttPublish(distance, temperature, humidity);
  }

  char float_string[6];

  // Print a message to the LCD.
  dtostrf(temperature, 2, 2, float_string);
  setDisplayLine(LINE_TEMP, "Temp: %s", float_string);

  dtostrf(distance, 2, 2, float_string);
  setDisplayLine(LINE_DIST, "Dist: %s", float_string);

  updateDisplay();

  delay(20000);
}
