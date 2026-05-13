#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "inc/config.h"

typedef void (*SoilPublishCallback)(const char* topic, const char* payload, bool retained);

struct SoilSensorItem {
  String id;
  String name;
  int pin = -1;
  int rawValue = 0;
  int percentValue = 0;
  int dryValue = 3200;
  int wetValue = 1200;
};

class SoilSensor {
public:
  void begin(SoilPublishCallback publishCallback);
  void loop();

  bool createFromJson(const String& json);
  bool updateFromJson(const String& json);
  bool deleteFromJson(const String& json);

  void publishList();
  void publishState(int index);

private:
  SoilSensorItem soilItems[MAX_SOIL_SENSORS]; 
  int sensorCount = 0;

  SoilPublishCallback mqttPublish = nullptr;
  unsigned long lastRead = 0;

  bool parseSensor(JsonObject obj, SoilSensorItem& item);
  bool isValidAdc1Pin(int pin);
  int findIndexById(const String& id);

  void readSensor(int index);
  void loadFromStorage();
  void saveToStorage();
};