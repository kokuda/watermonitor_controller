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

static Adafruit_MQTT_Subscribe mqttInterval = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/watermonitor.interval");
static Adafruit_MQTT_Publish mqttIntervalRequest = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/watermonitor.interval/get");

static int16_t _interval = 0;

static void intervalCallback(uint32_t interval) {
    Serial.printf("Interval is %u\r\n", interval);
    if (interval >= 1000 && interval < INT16_MAX) {
        _interval = (int16_t)interval;
    } else {
        Serial.println(F("Interval is invalid for int16_t"));
    }
}

void adafruitMqttSetup() {
    mqttInterval.setCallback(intervalCallback);
    mqtt.subscribe(&mqttInterval);
}

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

    // Publishing to feeds/xxx/get will trigger an update to feeds/xxx
    // https://io.adafruit.com/api/docs/mqtt.html#using-the-get-topic
    mqttIntervalRequest.publish("");

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

    if (distance > 0) {
        if (mqttDistance.publish(distance)) {
            Serial.println(F("Sending distance..."));
        }
    }

}

void adafruitMqttProcessPackets(uint32_t defaultInterval) {
    
    uint32_t interval = _interval >= 1000 ? _interval : defaultInterval;

    mqtt.processPackets(interval);
    if(! mqtt.ping()) {
        mqtt.disconnect();
    }
}
