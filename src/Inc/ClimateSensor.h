#pragma once

class ClimateSensor {
public:
  void begin();
  void loop();

  float getTemperature();
  float getHumidity();
  float getPressure();
  bool hasValidData();

private:
  float temperature = 0;
  float humidity = 0;
  float pressure = 0;
  bool valid = false;
  unsigned long lastRead = 0;
};