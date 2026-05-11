#pragma once

#include <WiFi.h>
#include <PubSubClient.h>

typedef void (*MqttMessageCallback)(char* topic, byte* payload, unsigned int length);

class MqttManager {
public:
  MqttManager();

  void begin(MqttMessageCallback callback);
  void loop(bool wifiConnected);

  bool isConnected();
  void publish(const char* topic, const char* payload, bool retained = false);

private:
  WiFiClient wifiClient;
  PubSubClient client;

  unsigned long lastReconnectAttempt = 0;

  bool reconnect();
};