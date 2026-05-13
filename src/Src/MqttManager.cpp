#include "inc/MqttManager.h"
#include "inc/config.h"

MqttManager::MqttManager() : client(wifiClient) {}

void MqttManager::begin(MqttMessageCallback callback) {
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
  client.setSocketTimeout(1);

   client.setBufferSize(2048);
}

void MqttManager::loop(bool wifiConnected) {
  if (!wifiConnected) return;

  if (!client.connected()) {
    unsigned long now = millis();

    if (now - lastReconnectAttempt >= MQTT_RETRY_INTERVAL) {
      lastReconnectAttempt = now;
      reconnect();
    }

    return;
  }

  client.loop();
}

bool MqttManager::reconnect() {
  if (client.connect(MQTT_CLIENT_ID, TOPIC_STATUS, 0, true, "offline")) {
    client.publish(TOPIC_STATUS, "online", true);

    client.subscribe(TOPIC_DEVICES_CREATE);
    client.subscribe(TOPIC_DEVICES_UPDATE);
    client.subscribe(TOPIC_DEVICES_DELETE);
    client.subscribe(TOPIC_DEVICES_SET);
    client.subscribe(TOPIC_DEVICES_REQUEST_LIST);
    client.subscribe(TOPIC_SOIL_CREATE);
    client.subscribe(TOPIC_SOIL_UPDATE);
    client.subscribe(TOPIC_SOIL_DELETE);
    client.subscribe(TOPIC_SOIL_REQUEST_LIST);

    return true;
  }

  return false;
}

bool MqttManager::isConnected() {
  return client.connected();
}

void MqttManager::publish(const char* topic, const char* payload, bool retained) {
  if (client.connected()) {
    client.publish(topic, payload, retained);
  }
}
