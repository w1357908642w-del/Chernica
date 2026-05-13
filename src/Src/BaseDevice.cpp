#include "inc/BaseDevice.h"

BaseDevice::BaseDevice(DeviceConfig config) {
  cfg = config;
}

int dateToDays(const String& date) {
  if (date.length() < 10) return 0;

  int y = date.substring(0, 4).toInt();
  int m = date.substring(5, 7).toInt();
  int d = date.substring(8, 10).toInt();

  return y * 365 + m * 31 + d;
}

void BaseDevice::loop(const String& currentDate, const String& currentTime) {
  if (cfg.mode != "AUTO") return;

  if (shouldTurnOn(currentDate, currentTime)) {
    if (applyState("ON")) {
      cfg.state = "ON";
      cfg.lastTurnOnDate = currentDate;
    }
  }

  if (shouldTurnOff(currentTime)) {
    if (applyState("OFF")) {
      cfg.state = "OFF";
    }
  }
}

bool BaseDevice::shouldTurnOn(const String& currentDate, const String& currentTime) {
  if (cfg.state == "ON") return false;
  if (currentTime != cfg.turnOnTime) return false;
  if (cfg.lastTurnOnDate == currentDate) return false;

  if (cfg.lastTurnOnDate.length() == 0) return true;

  int daysPassed = dateToDays(currentDate) - dateToDays(cfg.lastTurnOnDate);

  return daysPassed >= cfg.repeatEveryDays;
}

bool BaseDevice::shouldTurnOff(const String& currentTime) {
  if (cfg.state != "ON") return false;

  // обычный период: 08:00 → 22:00
  if (cfg.turnOnTime < cfg.turnOffTime) {
    return currentTime >= cfg.turnOffTime;
  }

  // период через полночь: 23:00 → 01:00
  if (cfg.turnOnTime > cfg.turnOffTime) {
    return currentTime >= cfg.turnOffTime && currentTime < cfg.turnOnTime;
  }

  return false;
}

void BaseDevice::setManualState(const String& state) {
  if (state != "ON" && state != "OFF") return;

  if (applyState(state)) {
    cfg.state = state;
  }
}

String BaseDevice::getId() {
  return cfg.id;
}

String BaseDevice::getState() {
  return cfg.state;
}

DeviceType BaseDevice::getType() {
  return cfg.type;
}

void BaseDevice::setConfig(DeviceConfig config) {
  cfg = config;
}

DeviceConfig BaseDevice::getConfig() {
  return cfg;
}

void BaseDevice::toJson(JsonObject obj) {
  obj["id"] = cfg.id;
  obj["name"] = cfg.name;
  obj["type"] = cfg.type == DEVICE_GPIO ? "GPIO" : "WIFI_RELAY";
  obj["state"] = cfg.state;
  obj["mode"] = cfg.mode;
  obj["turnOnTime"] = cfg.turnOnTime;
  obj["turnOffTime"] = cfg.turnOffTime;
  obj["lastTurnOnDate"] = cfg.lastTurnOnDate;
  obj["repeatEveryDays"] = cfg.repeatEveryDays;

  if (cfg.type == DEVICE_GPIO) {
    obj["pin"] = cfg.pin;
    obj["activeHigh"] = cfg.activeHigh;
  }

  if (cfg.type == DEVICE_WIFI_RELAY) {
    obj["ip"] = cfg.ip;
  }
}