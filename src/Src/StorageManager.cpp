#include "inc/StorageManager.h"

void StorageManager::begin() {
  preferences.begin("haus", false);
}

String StorageManager::loadDevicesJson() {
  return preferences.getString("devices", "[]");
}

void StorageManager::saveDevicesJson(const String& json) {
  preferences.putString("devices", json);
}