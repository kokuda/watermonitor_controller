#include "wifi.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

bool connectWifi(const char *wifi_ssid, const char *wifi_passphrase)
{

    Serial.print(F("[SETUP]Connect Wifi: "));
    Serial.print(wifi_ssid);

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(wifi_ssid, wifi_passphrase);

    int retries = 10;
    while (WiFiMulti.run() != WL_CONNECTED)
    {
        delay(2000);
        Serial.print(F("."));

        // Die if this takes too long.
        if (retries-- <= 0)
        {
            Serial.println(F("[SETUP]Error connecting to Wifi"));
            return false;
        }
    }

    Serial.println();
    Serial.println(F("[SETUP]Connected to Wifi"));
    return true;
}
