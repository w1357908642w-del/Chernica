#include "inc/TimeSensor.h"
#include <Wire.h>
#include <RTClib.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

WiFiUDP ntpUDP;
NTPClient ntpClient(ntpUDP, "pool.ntp.org", 4 * 3600, 60000);
RTC_DS3231 rtc;

bool ntpStarted = false;
unsigned long lastNtpSync = 0;

void TimeSensor::begin() {
  bool ntpStarted = false;
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

String TimeSensor::getDateTime() {
  if (!ready) return "";

  DateTime now = rtc.now();

  char buffer[25];
  snprintf(
    buffer,
    sizeof(buffer),
    "%04d-%02d-%02d %02d:%02d:%02d",
    now.year(),
    now.month(),
    now.day(),
    now.hour(),
    now.minute(),
    now.second()
  );

  return String(buffer);
}

void TimeSensor::syncFromNtpIfNeeded(bool wifiConnected) {
  if (!wifiConnected) return;

  if (!ntpStarted) {
    ntpClient.begin();
    ntpStarted = true;
  }

  unsigned long nowMs = millis();

  if (lastNtpSync != 0 && nowMs - lastNtpSync < 86400000UL) return;

  if (!ntpClient.update()) {
    ntpClient.forceUpdate();
  }

  time_t epoch = ntpClient.getEpochTime();
  struct tm* tmInfo = localtime(&epoch);

  if (!tmInfo) return;

  setDateTime(
    tmInfo->tm_year + 1900,
    tmInfo->tm_mon + 1,
    tmInfo->tm_mday,
    tmInfo->tm_hour,
    tmInfo->tm_min,
    tmInfo->tm_sec
  );

  lastNtpSync = nowMs;
}

void TimeSensor::setDateTime(int year, int month, int day, int hour, int minute, int second) {
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
}

String TimeSensor::getDate() {
  DateTime now = rtc.now();

  char buffer[16];

  sprintf(
    buffer,
    "%04d-%02d-%02d",
    now.year(),
    now.month(),
    now.day()
  );

  return String(buffer);
}

String TimeSensor::getTime() {
  DateTime now = rtc.now();

  char buffer[16];

  sprintf(
    buffer,
    "%02d:%02d",
    now.hour(),
    now.minute()
  );

  return String(buffer);
}