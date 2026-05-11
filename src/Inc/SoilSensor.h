#pragma once

class SoilSensor {
public:
  void begin();
  void loop();

  int getRaw();
  int getPercent();

private:
  int raw = 0;
  int percent = 0;
  unsigned long lastRead = 0;
};