#include "inc/SoilSensor.h"
#include "inc/config.h"
#include <Arduino.h>

#define SOIL_DRY_VALUE 3200
#define SOIL_WET_VALUE 1200

void SoilSensor::begin() {
  pinMode(SOIL_PIN, INPUT);
}

void SoilSensor::loop() {
  unsigned long now = millis();

  if (now - lastRead < SENSOR_READ_INTERVAL) return;
  lastRead = now;

  raw = analogRead(SOIL_PIN);

  percent = map(raw, SOIL_DRY_VALUE, SOIL_WET_VALUE, 0, 100);
  percent = constrain(percent, 0, 100);
}

int SoilSensor::getRaw() {
  return raw;
}

int SoilSensor::getPercent() {
  return percent;
}