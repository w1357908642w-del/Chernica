#pragma once
#include <Arduino.h>

enum DeviceType {
  DEVICE_GPIO,
  DEVICE_WIFI_RELAY
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
};