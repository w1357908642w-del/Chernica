#pragma once

#include <Arduino.h>

class WIFIManager {
public:
  void begin();
  void handle();

  bool isSetupMode();
  bool isConnected();

private:
  bool setupMode = false;
  unsigned long buttonPressedAt = 0;

  void connectSavedWiFi();
  void startSetupPortal();
  void handleSetupButton();

  String buildPage();
};