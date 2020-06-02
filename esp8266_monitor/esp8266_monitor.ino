/**
   esp8266_monitor.ino
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "NewPing.h"
// #include <FS.h>

// Install DHT Sensor library
// https://github.com/adafruit/DHT-sensor-library
#include <DHT.h>
#include <DHT_U.h>

ESP8266WiFiMulti WiFiMulti;

// http://wahiduddin.net/calc/density_algorithms.htm 
double dewPoint(double celsius, double humidity)
{
        double A0= 373.15/(273.15 + celsius);
        double SUM = -7.90298 * (A0-1);
        SUM += 5.02808 * log10(A0);
        SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
        SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
        SUM += log10(1013.246);
        double VP = pow(10, SUM-3) * humidity;
        double T = log(VP/0.61078);   // temp var
        return (241.88 * T) / (17.558-T);
}

#define DHTTYPE DHT11
#define DHTPIN  D1

DHT dht(DHTPIN, DHTTYPE);

void setupDHT11() {
  dht.begin();
}

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

const int TriggerPin = D3;
const int EchoPin = D2;
NewPing hcsr04(TriggerPin,EchoPin);

unsigned int getUltrasonicSensorPingTimeUs() {
  unsigned int ping_us = hcsr04.ping();
  unsigned int distance = 	(ping_us / US_ROUNDTRIP_CM);

  Serial.print(F("Distance: ")); Serial.print(distance); Serial.println(F("[cm]"));

  return ping_us;
}

bool loadConfig(JsonDocument& doc) {

  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  return true;

}

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
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

  // Use ..\esp8266_config to save the config to SPIFFS storage 
  StaticJsonDocument<200> doc;
  loadConfig(doc);
  const char* wifi_ssid = doc["wifi_ssid"];
  const char* wifi_passphrase = doc["wifi_passphrase"];

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(wifi_ssid, wifi_passphrase);

  setupDHT11();
}

void loop() {

  unsigned int ping_us = getUltrasonicSensorPingTimeUs();
  float temperature = getTemperature();
  // isnan(temperature) means read error
  float humidity = getHumidity();

  // wait for WiFi connection
  if (false && (WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://jigsaw.w3.org/HTTP/connection.html")) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }

  delay(5000);
}
