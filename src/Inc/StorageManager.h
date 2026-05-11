#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>

class StorageManager {
public:
  void begin();

  String loadDevicesJson();
  void saveDevicesJson(const String& json);

private:
  Preferences preferences;
};