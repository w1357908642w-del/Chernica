#include "inc/TimeSensor.h"

#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void TimeSensor::begin() {
  if (rtc.begin()) {
    ready = true;

    if (rtc.lostPower()) {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }
}

bool TimeSensor::isReady() {
  return ready;
}

String TimeSensor::getDate() {
  if (!ready) return "";

  DateTime now = rtc.now();

  char buffer[11];

  snprintf(
    buffer,
    sizeof(buffer),
    "%04d-%02d-%02d",
    now.year(),
    now.month(),
    now.day()
  );

  return String(buffer);
}

String TimeSensor::getTime() {
  if (!ready) return "";

  DateTime now = rtc.now();

  char buffer[6];

  snprintf(
    buffer,
    sizeof(buffer),
    "%02d:%02d",
    now.hour(),
    now.minute()
  );

  return String(buffer);
}