#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "abode.okuda.ca.rest.h"

void postToAbode(unsigned int ping_us, float temperature, float humidity) {

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
