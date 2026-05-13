#pragma once
#include <Arduino.h>

enum DeviceType {
  DEVICE_GPIO,
  DEVICE_WIFI_RELAY,
  DEVICE_SOIL_SENSOR
};

struct DeviceConfig {
  String id;
  String name;
  DeviceType type;

  String state = "OFF";
  String mode = "AUTO";

  String turnOnTime = "08:00";
  String turnOffTime = "22:00";

  String lastTurnOnDate = "";
  int repeatEveryDays = 1;

  int pin = -1;
  bool activeHigh = true;

  String ip = "";

  int rawValue = 0;
  int percentValue = 0;
  int dryValue = 3200;
  int wetValue = 1200;
};

