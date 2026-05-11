
#include "inc/WifiRelayDevice.h"
#include <WiFi.h>
#include <HTTPClient.h>

WifiRelayDevice::WifiRelayDevice(DeviceConfig config) : BaseDevice(config) {}

void WifiRelayDevice::begin() {
  applyState(cfg.state);
}

bool WifiRelayDevice::applyState(const String& state) {
  if (cfg.ip.length() == 0) return false;
  if (WiFi.status() != WL_CONNECTED) return false;

  return sendShellyRequest(state);
}

bool WifiRelayDevice::sendShellyRequest(const String& state) {
  String command;

  if (state == "ON") {
    command = "on";
  } else if (state == "OFF") {
    command = "off";
  } else {
    return false;
  }

  String url = "http://" + cfg.ip + "/relay/0?turn=" + command;

  HTTPClient http;
  http.setTimeout(1000);

  if (!http.begin(url)) {
    return false;
  }

  int code = http.GET();
  http.end();

  return code == 200;
}