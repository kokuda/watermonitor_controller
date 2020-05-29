// Storing the configuration specified in config.h into a file called "config.json"
// in the SPIFFS
//
// Uses ArduinoJson library by Benoit Blanchon.
// https://github.com/bblanchon/ArduinoJson
//

#include <ArduinoJson.h>
#include "FS.h"

// Define the BUILD_CONFIG macro in a file called "config.h"
// Do not check this file into git as it may contain passwords
// and sensitive information
//
// For example:
//
// #define BUILD_CONFIG(doc) \
//    doc["wifi_ssid"] = "MyWifi";    \
//    doc["wifi_passphrase"] = "password";
//
#include "config.h"

bool loadConfig() {
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

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  // Iterate over each member of the document and log it
  auto object = doc.as<JsonObject>();
  for (JsonPair keyValue : object) {
    Serial.print(keyValue.key().c_str());
    Serial.print(": ");
    if (keyValue.value().is<const char*>()) {
      Serial.println(keyValue.value().as<const char*>());
    } else {
      Serial.println("<Type not yet supported>");
    }
  }
  return true;
}

bool saveConfig() {
  StaticJsonDocument<200> doc;

  // Defined in datafile.h
  BUILD_CONFIG(doc);

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(doc, configFile);
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("");
  delay(1000);
  Serial.println("Mounting FS...");

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  delay(5000);

  if (!saveConfig()) {
    Serial.println("Failed to save config");
  } else {
    Serial.println("Config saved");
  }

  if (!loadConfig()) {
    Serial.println("Failed to load config");
  } else {
    Serial.println("Config loaded");
  }
}

void loop() {
}
