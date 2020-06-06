/**
   esp8266_monitor.ino
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "thermometer.h"
#include "ultrasonic.h"

ESP8266WiFiMulti WiFiMulti;

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

  Serial.print("[SETUP]Connect Wifi: ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(wifi_ssid, wifi_passphrase);

  setupThermometer();
}

void postData(unsigned int ping_us, float temperature, float humidity) {

  WiFiClient client;
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");

  if (http.begin(client, "http://abode.okuda.ca/api/waterlevels")) {  // HTTP

    StaticJsonDocument<200> doc;
    doc["temperature_dd"] = (int)round(temperature * 10);
    doc["flight_duration_us"] = ping_us;

    char* buf(new char[1024]);
    serializeJson(doc, buf, 1024);
    String s(buf);

    Serial.print("[HTTP] POST...\n");
    http.addHeader("Accept", "application/json");
    http.addHeader("Content-Type", "application/json");
    
    int httpCode = http.POST(s);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.printf("[HTTP] Unable to connect\n");
  }

}

void loop() {

  // wait for WiFi connection
  if (WiFiMulti.run() == WL_CONNECTED) {

    unsigned int ping_us = getUltrasonicSensorPingTimeUs();
    float temperature = getTemperature();
    float humidity = getHumidity();

    postData(ping_us, temperature, humidity);
  }

  delay(5000);
}
