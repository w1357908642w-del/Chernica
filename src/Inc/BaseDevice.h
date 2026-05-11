#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "DeviceConfig.h"

class BaseDevice {
public:
  BaseDevice(DeviceConfig config);

  virtual ~BaseDevice() {}

  virtual void begin() = 0;
  virtual bool applyState(const String& state) = 0;

  void loop(const String& currentDate, const String& currentTime);

  String getId();
  String getState();
  DeviceType getType();

  void setConfig(DeviceConfig config);
  DeviceConfig getConfig();

  void setManualState(const String& state);
  void toJson(JsonObject obj);

protected:
  DeviceConfig cfg;

  bool shouldTurnOn(const String& currentDate, const String& currentTime);
  bool shouldTurnOff(const String& currentTime);
};