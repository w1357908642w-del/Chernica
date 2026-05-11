#include "inc/WiFiManager.h"
#include "inc/config.h"
#include <WiFi.h>
#include <Arduino.h>

void WiFiManagerCustom::begin() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void WiFiManagerCustom::loop() {
  if (WiFi.status() == WL_CONNECTED) return;

  unsigned long now = millis();

  if (now - lastReconnectAttempt >= WIFI_RETRY_INTERVAL) {
    lastReconnectAttempt = now;

    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
}

bool WiFiManagerCustom::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}