#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "inc/config.h"
#include "BaseDevice.h"
#include "GpioDevice.h"
#include "WifiRelayDevice.h"
#include "StorageManager.h"

typedef void (*DevicePublishCallback)(const char* topic, const char* payload, bool retained);

class DeviceManager {
public:
  void begin(StorageManager* storage, DevicePublishCallback publishCallback);
  void loop(const String& currentDate, const String& currentTime);

  bool createDeviceFromJson(const String& json);
  bool updateDeviceFromJson(const String& json);
  bool deleteDeviceFromJson(const String& json);
  bool setDeviceStateFromJson(const String& json);

  void publishList();
  void publishDeviceState(BaseDevice* device);

private:
  BaseDevice* devices[MAX_DEVICES];
  int deviceCount = 0;

  StorageManager* storageManager = nullptr;
  DevicePublishCallback mqttPublish = nullptr;

  BaseDevice* createDevice(DeviceConfig cfg);
  BaseDevice* findDevice(const String& id);

  bool parseConfig(JsonObject obj, DeviceConfig& cfg);
  void saveToStorage();
  void clearDevices();

  int countByType(DeviceType type);
};