#pragma once

#include <Arduino.h>

class TimeSensor {
public:
  void begin();
  bool isReady();

  String getDate();
  String getTime();

private:
  bool ready = false;
};