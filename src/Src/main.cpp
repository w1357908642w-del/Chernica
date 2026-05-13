#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>

#include "inc/config.h"
#include "inc/WiFiManager.h"
#include "inc/MqttManager.h"
#include "inc/ClimateSensor.h"
#include "inc/SoilSensor.h"
#include "inc/TimeSensor.h"
#include "inc/StorageManager.h"
#include "inc/DeviceManager.h"

WiFiManagerCustom wifiManager;
MqttManager mqttManager;
StorageManager storageManager;
DeviceManager deviceManager;
TimeSensor timeSensor;
ClimateSensor climateSensor;
SoilSensor soilSensor;



void blinkLed() {
    static uint32_t t;
    static bool s;

    if (millis() - t > 1000) {
        t = millis();
        digitalWrite(2, s = !s);
    }
}

unsigned long lastDeviceTick = 0;
unsigned long lastMetricsSend = 0;
bool listPublishedAfterConnect = false;

void mqttPublishCallback(const char* topic, const char* payload, bool retained) {
  mqttManager.publish(topic, payload, retained);
}

bool checkAuth(JsonDocument& doc) {
  const char* login = doc["auth"]["login"] | "";
  const char* password = doc["auth"]["password"] | "";

  return String(login) == DEVICE_LOGIN && String(password) == DEVICE_PASSWORD;
}

String jsonObjectToString(JsonObject obj) {
  String result;
  serializeJson(obj, result);
  return result;
}

void handleMqttMessage(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("MQTT IN: ");
  Serial.print(topic);
  Serial.print(" ");
  Serial.println(message);

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  StaticJsonDocument<2048> doc;

  if (deserializeJson(doc, message)) {
    return;
  }

  if (!checkAuth(doc)) {
    return;
  }

  String topicStr = String(topic);

  if (topicStr == TOPIC_DEVICES_CREATE) {
    JsonObject device = doc["device"];
    deviceManager.createDeviceFromJson(jsonObjectToString(device));
  }

  if (topicStr == TOPIC_DEVICES_UPDATE) {
    JsonObject device = doc["device"];
    deviceManager.updateDeviceFromJson(jsonObjectToString(device));
  }

  if (topicStr == TOPIC_DEVICES_DELETE) {
    StaticJsonDocument<128> d;
    d["id"] = doc["id"] | "";

    String json;
    serializeJson(d, json);

    deviceManager.deleteDeviceFromJson(json);
  }

  if (topicStr == TOPIC_DEVICES_SET) {
    StaticJsonDocument<128> d;
    d["id"] = doc["id"] | "";
    d["state"] = doc["state"] | "";

    String json;
    serializeJson(d, json);

    deviceManager.setDeviceStateFromJson(json);
  }

  if (topicStr == TOPIC_DEVICES_REQUEST_LIST) {
    deviceManager.publishList();
  }
  if (topicStr == TOPIC_SOIL_CREATE) {
  JsonObject sensor = doc["sensor"];
  soilSensor.createFromJson(jsonObjectToString(sensor));
}

if (topicStr == TOPIC_SOIL_UPDATE) {
  JsonObject sensor = doc["sensor"];
  soilSensor.updateFromJson(jsonObjectToString(sensor));
}

if (topicStr == TOPIC_SOIL_DELETE) {
  StaticJsonDocument<128> d;
  d["id"] = doc["id"] | "";

  String json;
  serializeJson(d, json);

  soilSensor.deleteFromJson(json);
}

if (topicStr == TOPIC_SOIL_REQUEST_LIST) {
  soilSensor.publishList();
}
}

void sendSensors() {
  if (!mqttManager.isConnected()) return;

  StaticJsonDocument<512> doc;

  doc["auth"]["login"] = DEVICE_LOGIN;
  doc["auth"]["password"] = DEVICE_PASSWORD;
  doc["device"] = MQTT_CLIENT_ID;

  doc["temperature"] = climateSensor.getTemperature();
  doc["humidity"] = climateSensor.getHumidity();
  doc["pressure"] = climateSensor.getPressure();

  if (timeSensor.isReady()) {
    doc["time"] = timeSensor.getDate() + " " + timeSensor.getTime();
  }

  char buffer[512];
  serializeJson(doc, buffer);

  mqttManager.publish(TOPIC_SENSORS, buffer, false);
}

void sendStatus() {
  if (!mqttManager.isConnected()) return;

  StaticJsonDocument<256> doc;

  doc["auth"]["login"] = DEVICE_LOGIN;
  doc["auth"]["password"] = DEVICE_PASSWORD;
  doc["device"] = MQTT_CLIENT_ID;
  doc["status"] = "online";

  char buffer[256];
  serializeJson(doc, buffer);

  mqttManager.publish(TOPIC_STATUS, buffer, true);
}

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);

  storageManager.begin();
  timeSensor.begin();

  climateSensor.begin();
  soilSensor.begin(mqttPublishCallback);

  wifiManager.begin();
  mqttManager.begin(handleMqttMessage);

  deviceManager.begin(&storageManager, mqttPublishCallback);

  pinMode(2,OUTPUT);

  Serial.println("BOOT");
  Serial.println("Storage begin");
  Serial.println("Sensors begin");
  Serial.println("WiFi begin");
  Serial.println("MQTT begin");
  Serial.println("Setup done");
}

void loop() {
  
  blinkLed();

  wifiManager.loop();
  mqttManager.loop(wifiManager.isConnected());

  if (mqttManager.isConnected() && !listPublishedAfterConnect) {
    sendStatus();
    deviceManager.publishList();
    listPublishedAfterConnect = true;
  }

  if (!mqttManager.isConnected()) {
    listPublishedAfterConnect = false;
  }

  unsigned long now = millis();

  climateSensor.loop();
  soilSensor.loop();

  if (now - lastDeviceTick >= DEVICE_TICK_INTERVAL) {
    lastDeviceTick = now;

    if (timeSensor.isReady()) {
      deviceManager.loop(timeSensor.getDate(), timeSensor.getTime());
    }
  }

  if (now - lastMetricsSend >= METRICS_SEND_INTERVAL) {
    lastMetricsSend = now;
    sendSensors();
  
  }

  timeSensor.syncFromNtpIfNeeded(wifiManager.isConnected());
}


