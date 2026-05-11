#include "inc/GpioDevice.h"
#include <Arduino.h>

GpioDevice::GpioDevice(DeviceConfig config) : BaseDevice(config) {}

void GpioDevice::begin() {
  pinMode(cfg.pin, OUTPUT);
  applyState(cfg.state);
}

bool GpioDevice::applyState(const String& state) {
  if (cfg.pin < 0) return false;

  bool on = state == "ON";

  if (cfg.activeHigh) {
    digitalWrite(cfg.pin, on ? HIGH : LOW);
  } else {
    digitalWrite(cfg.pin, on ? LOW : HIGH);
  }

  return true;
}