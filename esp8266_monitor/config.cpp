#include <Arduino.h>
#include <FS.h>
#include "config.h"

bool setupConfigFile() {
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    false;
  }
  return true;
}

std::unique_ptr<char[]> loadConfigFile() {

  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return std::unique_ptr<char[]>();
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return std::unique_ptr<char[]>();
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  return buf;
}
