#pragma once

#include "BaseDevice.h"

class WifiRelayDevice : public BaseDevice {
public:
  WifiRelayDevice(DeviceConfig config);

  void begin() override;
  bool applyState(const String& state) override;

private:
  bool sendShellyRequest(const String& state);
};