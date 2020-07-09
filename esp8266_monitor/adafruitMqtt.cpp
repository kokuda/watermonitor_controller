#include <Arduino.h>
#include <WiFiClient.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Define the following macros in a file called "configAdafruitMqtt.h"
// Do not check this file into git as it may contain passwords
// and sensitive information
//
// #define AIO_SERVER      "io.adafruit.com"
// #define AIO_USERNAME    "Your Adafruit account name"
// #define AIO_KEY         "Your Adafruit key"
// #define AIO_SERVERPORT  1883                   
//
#include "configAdafruitMqtt.h"

static WiFiClient client;
static Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
static Adafruit_MQTT_Publish mqttTemperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/watermonitor.temperature");
static Adafruit_MQTT_Publish mqttHumidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/watermonitor.humidity");
static Adafruit_MQTT_Publish mqttDistance = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/watermonitor.distance");

bool adafruitMqttConnect() {

    int8_t ret;

    if (mqtt.connected()) {      // Stop if already connected.
        return true;
    }

    Serial.print(F("Connecting to MQTT... "));
    uint8_t retries = 3;
    while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
        Serial.println(mqtt.connectErrorString(ret));
        Serial.println(F("Retrying MQTT connection in 5 seconds..."));
        mqtt.disconnect();
        delay(5000);  // wait 5 seconds
        retries--;
        if (retries == 0) {
            Serial.println(F("MQTT Unable to connect!"));
            return false;
        }
    }
    Serial.println(F("MQTT Connected!"));
    return true;
}

void adafruitMqttPublish(float distance, float temperature, float humidity) {
    if (mqttTemperature.publish(temperature)) 
    {
        Serial.println(F("Sending temperature..."));
    }

    if (mqttHumidity.publish(humidity)) {
        Serial.println(F("Sending humidity..."));
    }

    if (mqttDistance.publish(distance)) {
        Serial.println(F("Sending distance..."));
    }

}