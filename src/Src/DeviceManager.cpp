#include "inc/DeviceManager.h"
#include "inc/config.h"

void DeviceManager::begin(StorageManager* storage, DevicePublishCallback publishCallback) {
  storageManager = storage;
  mqttPublish = publishCallback;

  String json = storageManager->loadDevicesJson();

    DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, json);

  if (error || !doc.is<JsonArray>()) {
    saveToStorage();
    return;
  }

  JsonArray arr = doc.as<JsonArray>();

  for (JsonObject obj : arr) {
    DeviceConfig cfg;

    if (parseConfig(obj, cfg)) {
      BaseDevice* device = createDevice(cfg);

      if (device && deviceCount < MAX_DEVICES) {
        devices[deviceCount++] = device;
        device->begin();
      }
    }
  }
}

void DeviceManager::loop(const String& currentDate, const String& currentTime) {
  for (int i = 0; i < deviceCount; i++) {
    String before = devices[i]->getState();

    devices[i]->loop(currentDate, currentTime);

    if (before != devices[i]->getState()) {
      publishDeviceState(devices[i]);
      saveToStorage();
    }
  }
}

bool DeviceManager::createDeviceFromJson(const String& json) {
  StaticJsonDocument<1024> doc;
  if (deserializeJson(doc, json)) return false;

  DeviceConfig cfg;
  if (!parseConfig(doc.as<JsonObject>(), cfg)) return false;
  if (findDevice(cfg.id)) return false;
  if (deviceCount >= MAX_DEVICES) return false;

  if (cfg.type == DEVICE_GPIO && countByType(DEVICE_GPIO) >= MAX_GPIO_DEVICES) return false;
  if (cfg.type == DEVICE_WIFI_RELAY && countByType(DEVICE_WIFI_RELAY) >= MAX_WIFI_RELAY_DEVICES) return false;

  BaseDevice* device = createDevice(cfg);
  if (!device) return false;

  devices[deviceCount++] = device;
  device->begin();

  saveToStorage();
  publishList();

  return true;
}

bool DeviceManager::updateDeviceFromJson(const String& json) {
  StaticJsonDocument<1024> doc;
  if (deserializeJson(doc, json)) return false;

  DeviceConfig cfg;
  if (!parseConfig(doc.as<JsonObject>(), cfg)) return false;

  BaseDevice* oldDevice = findDevice(cfg.id);
  if (!oldDevice) return false;

  deleteDeviceFromJson("{\"id\":\"" + cfg.id + "\"}");
  return createDeviceFromJson(json);
}

bool DeviceManager::deleteDeviceFromJson(const String& json) {
  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, json)) return false;

  String id = doc["id"] | "";
  if (id.length() == 0) return false;

  for (int i = 0; i < deviceCount; i++) {
    if (devices[i]->getId() == id) {
      devices[i]->setManualState("OFF");
      delete devices[i];

      for (int j = i; j < deviceCount - 1; j++) {
        devices[j] = devices[j + 1];
      }

      deviceCount--;

      saveToStorage();
      publishList();

      return true;
    }
  }

  return false;
}

bool DeviceManager::setDeviceStateFromJson(const String& json) {
  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, json)) return false;

  String id = doc["id"] | "";
  String state = doc["state"] | "";

  BaseDevice* device = findDevice(id);
  if (!device) return false;

  device->setManualState(state);

  saveToStorage();
  publishDeviceState(device);
  publishList();

  return true;
}

void DeviceManager::publishList() {
  if (!mqttPublish) return;

  DynamicJsonDocument doc(8192);

  doc["auth"]["login"] = DEVICE_LOGIN;
  doc["auth"]["password"] = DEVICE_PASSWORD;
  doc["device"] = DEVICE_LOGIN;

  JsonArray arr = doc.createNestedArray("devices");

  for (int i = 0; i < deviceCount; i++) {
    JsonObject obj = arr.createNestedObject();
    devices[i]->toJson(obj);
  }

String payload;
serializeJson(doc, payload);

mqttPublish(TOPIC_DEVICES_LIST, payload.c_str(), true);
}

void DeviceManager::publishDeviceState(BaseDevice* device) {
  if (!mqttPublish || !device) return;

  StaticJsonDocument<1024> doc;

  doc["auth"]["login"] = DEVICE_LOGIN;
  doc["auth"]["password"] = DEVICE_PASSWORD;
  doc["device"] = DEVICE_LOGIN;

  JsonObject obj = doc.createNestedObject("deviceData");
  device->toJson(obj);

  char buffer[1024];
  serializeJson(doc, buffer);

  mqttPublish(TOPIC_DEVICES_STATE, buffer, true);
}

BaseDevice* DeviceManager::createDevice(DeviceConfig cfg) {
  if (cfg.type == DEVICE_GPIO) {
    return new GpioDevice(cfg);
  }

  if (cfg.type == DEVICE_WIFI_RELAY) {
    return new WifiRelayDevice(cfg);
  }

  return nullptr;
}

BaseDevice* DeviceManager::findDevice(const String& id) {
  for (int i = 0; i < deviceCount; i++) {
    if (devices[i]->getId() == id) {
      return devices[i];
    }
  }

  return nullptr;
}

bool DeviceManager::parseConfig(JsonObject obj, DeviceConfig& cfg) {
  cfg.id = obj["id"] | "";
  cfg.name = obj["name"] | "";

  String type = obj["type"] | "";

  if (cfg.id.length() == 0 || cfg.name.length() == 0) return false;

  if (type == "GPIO") {
    cfg.type = DEVICE_GPIO;
  } else if (type == "WIFI_RELAY") {
    cfg.type = DEVICE_WIFI_RELAY;
  } else {
    return false;
  }

  cfg.state = obj["state"] | "OFF";
  cfg.mode = obj["mode"] | "AUTO";
  cfg.turnOnTime = obj["turnOnTime"] | "08:00";
  cfg.turnOffTime = obj["turnOffTime"] | "22:00";
  cfg.lastTurnOnDate = obj["lastTurnOnDate"] | "";
  cfg.repeatEveryDays = obj["repeatEveryDays"] | 1;

  cfg.pin = obj["pin"] | -1;
  cfg.activeHigh = obj["activeHigh"] | true;
  cfg.ip = obj["ip"] | "";

  if (cfg.type == DEVICE_GPIO && cfg.pin < 0) return false;
  if (cfg.type == DEVICE_WIFI_RELAY && cfg.ip.length() == 0) return false;

  return true;
}

void DeviceManager::saveToStorage() {
  if (!storageManager) return;

  DynamicJsonDocument doc(8192);
  JsonArray arr = doc.to<JsonArray>();

  for (int i = 0; i < deviceCount; i++) {
    JsonObject obj = arr.createNestedObject();
    devices[i]->toJson(obj);
  }

  String json;
  serializeJson(doc, json);

  storageManager->saveDevicesJson(json);
}

int DeviceManager::countByType(DeviceType type) {
  int count = 0;

  for (int i = 0; i < deviceCount; i++) {
    if (devices[i]->getType() == type) {
      count++;
    }
  }

  return count;
}