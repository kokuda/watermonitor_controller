/**
   watermonitor_controller.ino
*/
///////////////////////////////////////////////////////////////////////////////

// Enable or disable writing to the LCD
#define ENABLE_LCD 1

// Enable OTA support (experimental, not yet working)`
#define ENABLE_OTA 0

// Write telemetry to abode.okuda.ca REST API
#define ENABLE_ABODE_OKUDA_CA 0

#define DEFAULT_MQTT_INTERVAL 20000

///////////////////////////////////////////////////////////////////////////////

// Define the following macros in a file called "configData.h"
// Do not check this file into git as it may contain passwords
// and sensitive information
//
// WIFI_SSID - string containing the ssid of the wifi_passphrase
// WIFI_PASSPHREASE - The password of the ssid 
//
// For example:
//
// #define WIFI_SSID "MyWifi"
// #define WIFI_PASSPHRASE "password"
//
#include "configData.h"
#ifndef WIFI_SSID
  #error "WIFI_SSID is not defined"
#endif
#ifndef WIFI_PASSPHRASE
  #error "WIFI_PASSPHRASE is not defined"
#endif

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "thermometer.h"
#include "ultrasonic.h"
#include "adafruitMqtt.h"
#include "wifi.h"

#if ENABLE_LCD
  #include "display.h"
#else
  #define initDisplay()
  #define updateDisplay()
  #define setDisplayLine(...)
#endif

#if ENABLE_ABODE_OKUDA_CA
  #include "abode.okuda.ca.rest.h"
#else
  #define postToAbode(...)
#endif

#if ENABLE_OTA
  #include "otaupdate.h"
#else
  #define otaSetup()
  #define otaUpdate()
#endif


#define LINE_MESSAGE 0
#define LINE_TEMP_DIST 1


void connectWifiFromConfig() {

  const char* wifi_ssid = WIFI_SSID;
  const char* wifi_passphrase = WIFI_PASSPHRASE;

  setDisplayLine(LINE_MESSAGE, "Connecting...");
  updateDisplay();
  delay(1000);

  if (!connectWifi(wifi_ssid, wifi_passphrase)) {
    setDisplayLine(LINE_MESSAGE, "WiFi error");
    updateDisplay();
    delay(10000);
    ESP.restart();
    while (1);
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  setDisplayLine(LINE_MESSAGE, "%s", WiFi.SSID().c_str());

}

bool reconnectWifiFromConfig() {

  setDisplayLine(LINE_MESSAGE, "Disconnected...");
  updateDisplay();

  // Wait before we connect to limit rate between attempts
  delay(5000);

  // Test it again after waiting, just in case.
  if (!WiFi.isConnected()) {
    WiFi.disconnect();
    connectWifiFromConfig();
  }

  if (WiFi.isConnected()) {
    return true;
  }

  return false;
}

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

#if ENABLE_CONFIG_FILE
  if (!setupConfigFile()) {
    return;
  }
#endif

  initDisplay();

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    setDisplayLine(LINE_MESSAGE, "WAIT %d", t);
    updateDisplay();
    Serial.flush();
    delay(1000);
  }

  connectWifiFromConfig();

  setupThermometer();

  adafruitMqttSetup();

  otaSetup();

}

void loop() {

  if (!WiFi.isConnected()) {
    Serial.println(F("Disconnected from WiFi, reconnecting..."));
    if (!reconnectWifiFromConfig()) {
      return;
    }
  }

  unsigned int ping_us = getUltrasonicSensorPingTimeUs();
  float temperature = getTemperature();
  float calculationTemperature = isnan(temperature) ? 20.0 : temperature;
  float humidity = getHumidity();
  float distance = getDistance(ping_us, calculationTemperature);

  float validatedDistance = validateDistance(distance, millis());

  Serial.print(F("Distance: ")); Serial.print(distance); Serial.println(F("[cm]"));
  Serial.print(F("Validated Distance: ")); Serial.print(validatedDistance); Serial.println(F("[cm]"));

  postToAbode(ping_us, temperature, humidity);

  if (adafruitMqttConnect()) {
    adafruitMqttPublish(validatedDistance, temperature, humidity);
  }

  char temp_string[6];
  char dist_string[6];

  // Print a message to the LCD.
  dtostrf(temperature, 2, 2, temp_string);
  dtostrf(distance, 2, 2, dist_string);
  setDisplayLine(LINE_TEMP_DIST, "%s%c %scm", temp_string, (char)223, dist_string);

  updateDisplay();

  otaUpdate();

  // This function blocks until it is time for the next update.
  adafruitMqttProcessPackets(DEFAULT_MQTT_INTERVAL);
}
