#pragma once

class WiFiManagerCustom {
public:
  void begin();
  void loop();
  bool isConnected();

private:
  unsigned long lastReconnectAttempt = 0;
};