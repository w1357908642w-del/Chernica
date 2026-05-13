#pragma once
#include <WString.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

class TimeSensor {
public:
  void begin();
  bool isReady();
  void syncFromNtpIfNeeded(bool wifiConnected);
  void setDateTime(int year, int month, int day, int hour, int minute, int second);
  String getDateTime();
  String getDate();
  String getTime();

private:
  bool ready = false;
};