#include "inc/ClimateSensor.h"
#include "inc/config.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

void ClimateSensor::begin() {
  valid = bme.begin(0x76);

  if (!valid) {
    valid = bme.begin(0x77);
  }

  if (valid) {
    Serial.println("BME280 connected");
  } else {
    Serial.println("BME280 not found");
  }
}

void ClimateSensor::loop() {
  unsigned long now = millis();

  if (now - lastRead < SENSOR_READ_INTERVAL) return;
  lastRead = now;

  if (!valid) return;

  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure() / 100.0F;

  if (
    !isnan(t) &&
    !isnan(h) &&
    !isnan(p) &&
    t > -40 && t < 85 &&
    h >= 0 && h <= 100 &&
    p > 300 && p < 1100
  ) {
    temperature = t;
    humidity = h;
    pressure = p;
    valid = true;
  } else {
    valid = false;
  }
}

float ClimateSensor::getTemperature() {
  return temperature;
}

float ClimateSensor::getHumidity() {
  return humidity;
}

float ClimateSensor::getPressure() {
  return pressure;
}

bool ClimateSensor::hasValidData() {
  return valid;
}