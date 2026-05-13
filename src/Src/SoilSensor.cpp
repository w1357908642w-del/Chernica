#include "inc/SoilSensor.h"
#include "inc/config.h"

#include <Arduino.h>
#include <Preferences.h>

Preferences soilPrefs;

void SoilSensor::begin(SoilPublishCallback publishCallback) {
  mqttPublish = publishCallback;

  soilPrefs.begin("soil", false);

  loadFromStorage();

  for (int i = 0; i < sensorCount; i++) {
    pinMode(soilItems[i].pin, INPUT);
    readSensor(i);
  }
}

void SoilSensor::loop() {
  unsigned long now = millis();

  if (now - lastRead < SENSOR_READ_INTERVAL) return;
  lastRead = now;

  for (int i = 0; i < sensorCount; i++) {
    readSensor(i);
    publishState(i);
  }
}

bool SoilSensor::createFromJson(const String& json) {
  if (sensorCount >= MAX_SOIL_SENSORS) return false;

  StaticJsonDocument<512> doc;
  if (deserializeJson(doc, json)) return false;

  SoilSensorItem item;

  if (!parseSensor(doc.as<JsonObject>(), item)) return false;
  if (findIndexById(item.id) != -1) return false;

  soilItems[sensorCount] = item;
  pinMode(item.pin, INPUT);
  readSensor(sensorCount);

  sensorCount++;

  saveToStorage();
  publishList();

  return true;
}

bool SoilSensor::updateFromJson(const String& json) {
  StaticJsonDocument<512> doc;
  if (deserializeJson(doc, json)) return false;

  SoilSensorItem item;

  if (!parseSensor(doc.as<JsonObject>(), item)) return false;

  int index = findIndexById(item.id);
  if (index == -1) return false;

  soilItems[index] = item;
  pinMode(item.pin, INPUT);
  readSensor(index);

  saveToStorage();
  publishList();

  return true;
}

bool SoilSensor::deleteFromJson(const String& json) {
  StaticJsonDocument<128> doc;
  if (deserializeJson(doc, json)) return false;

  String id = doc["id"] | "";

  int index = findIndexById(id);
  if (index == -1) return false;

  for (int i = index; i < sensorCount - 1; i++) {
    soilItems[i] = soilItems[i + 1];
  }

  sensorCount--;

  saveToStorage();
  publishList();

  return true;
}

void SoilSensor::readSensor(int index) {
  if (index < 0 || index >= sensorCount) return;

  SoilSensorItem& item = soilItems[index];

  if (!isValidAdc1Pin(item.pin)) return;

  item.rawValue = analogRead(item.pin);

  item.percentValue = map(
    item.rawValue,
    item.dryValue,
    item.wetValue,
    0,
    100
  );

  item.percentValue = constrain(item.percentValue, 0, 100);
}

void SoilSensor::publishList() {
  if (!mqttPublish) return;

  DynamicJsonDocument doc(4096);

  doc["auth"]["login"] = DEVICE_LOGIN;
  doc["auth"]["password"] = DEVICE_PASSWORD;
  doc["device"] = MQTT_CLIENT_ID;

  JsonArray arr = doc.createNestedArray("sensors");

  for (int i = 0; i < sensorCount; i++) {
    JsonObject obj = arr.createNestedObject();

    obj["id"] = soilItems[i].id;
    obj["name"] = soilItems[i].name;
    obj["pin"] = soilItems[i].pin;
    obj["rawValue"] = soilItems[i].rawValue;
    obj["percentValue"] = soilItems[i].percentValue;
    obj["dryValue"] = soilItems[i].dryValue;
    obj["wetValue"] = soilItems[i].wetValue;
  }

  String payload;
  serializeJson(doc, payload);

  mqttPublish(TOPIC_SOIL_LIST, payload.c_str(), true);
}

void SoilSensor::publishState(int index) {
  if (!mqttPublish) return;
  if (index < 0 || index >= sensorCount) return;

  StaticJsonDocument<512> doc;

  doc["auth"]["login"] = DEVICE_LOGIN;
  doc["auth"]["password"] = DEVICE_PASSWORD;
  doc["device"] = MQTT_CLIENT_ID;

  doc["sensor"]["id"] = soilItems[index].id;
  doc["sensor"]["name"] = soilItems[index].name;
  doc["sensor"]["pin"] = soilItems[index].pin;
  doc["sensor"]["rawValue"] = soilItems[index].rawValue;
  doc["sensor"]["percentValue"] = soilItems[index].percentValue;
  doc["sensor"]["dryValue"] = soilItems[index].dryValue;
  doc["sensor"]["wetValue"] = soilItems[index].wetValue;

  String payload;
  serializeJson(doc, payload);

  mqttPublish(TOPIC_SOIL_STATE, payload.c_str(), true);
}

bool SoilSensor::parseSensor(JsonObject obj, SoilSensorItem& item) {
  item.id = obj["id"] | "";
  item.name = obj["name"] | "";
  item.pin = obj["pin"] | -1;
  item.dryValue = obj["dryValue"] | 3200;
  item.wetValue = obj["wetValue"] | 1200;

  if (item.id.length() == 0) return false;
  if (item.name.length() == 0) return false;
  if (!isValidAdc1Pin(item.pin)) return false;

  return true;
}

bool SoilSensor::isValidAdc1Pin(int pin) {
  return pin == 32 ||
         pin == 33 ||
         pin == 34 ||
         pin == 35 ||
         pin == 36 ||
         pin == 39;
}

int SoilSensor::findIndexById(const String& id) {
  for (int i = 0; i < sensorCount; i++) {
    if (soilItems[i].id == id) return i;
  }

  return -1;
}

void SoilSensor::loadFromStorage() {
  String json = soilPrefs.getString("items", "[]");

  DynamicJsonDocument doc(4096);

  if (deserializeJson(doc, json)) return;
  if (!doc.is<JsonArray>()) return;

  JsonArray arr = doc.as<JsonArray>();

  sensorCount = 0;

  for (JsonObject obj : arr) {
    if (sensorCount >= MAX_SOIL_SENSORS) break;

    SoilSensorItem item;

    if (parseSensor(obj, item)) {
      soilItems[sensorCount++] = item;
    }
  }
}

void SoilSensor::saveToStorage() {
  DynamicJsonDocument doc(4096);
  JsonArray arr = doc.to<JsonArray>();

  for (int i = 0; i < sensorCount; i++) {
    JsonObject obj = arr.createNestedObject();

    obj["id"] = soilItems[i].id;
    obj["name"] = soilItems[i].name;
    obj["pin"] = soilItems[i].pin;
    obj["dryValue"] = soilItems[i].dryValue;
    obj["wetValue"] = soilItems[i].wetValue;
  }

  String json;
  serializeJson(doc, json);

  soilPrefs.putString("items", json);
}