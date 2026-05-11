#pragma once

#include "BaseDevice.h"

class GpioDevice : public BaseDevice {
public:
  GpioDevice(DeviceConfig config);

  void begin() override;
  bool applyState(const String& state) override;
};