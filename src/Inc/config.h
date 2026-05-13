#pragma once

#define WIFI_SSID "ded"
#define WIFI_PASSWORD "23456789"

#define MQTT_HOST "192.168.0.100"
#define MQTT_PORT 1883

#define MQTT_CLIENT_ID "esp32-2"
#define DEVICE_LOGIN "esp32_2"
#define DEVICE_PASSWORD "1234"

#define MAX_GPIO_DEVICES 8
#define MAX_WIFI_RELAY_DEVICES 10
#define MAX_DEVICES 18

#define SOIL_PIN 34
#define SENSOR_READ_INTERVAL 5000

#define MQTT_RETRY_INTERVAL 5000
#define WIFI_RETRY_INTERVAL 5000
#define DEVICE_TICK_INTERVAL 1000

#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

#define TOPIC_STATUS "home/esp32/status"

#define TOPIC_DEVICES_LIST "home/esp32/devices/list"
#define TOPIC_DEVICES_STATE "home/esp32/devices/state"

#define TOPIC_DEVICES_CREATE "home/esp32/devices/create"
#define TOPIC_DEVICES_UPDATE "home/esp32/devices/update"
#define TOPIC_DEVICES_DELETE "home/esp32/devices/delete"
#define TOPIC_DEVICES_SET "home/esp32/devices/set"
#define TOPIC_DEVICES_REQUEST_LIST "home/esp32/devices/request-list"

#define TOPIC_SENSORS "home/esp32/sensors"
#define METRICS_SEND_INTERVAL 5000

#define MAX_SOIL_SENSORS 6

#define TOPIC_SOIL_LIST "home/esp32/soil/list"
#define TOPIC_SOIL_STATE "home/esp32/soil/state"

#define TOPIC_SOIL_CREATE "home/esp32/soil/create"
#define TOPIC_SOIL_UPDATE "home/esp32/soil/update"
#define TOPIC_SOIL_DELETE "home/esp32/soil/delete"
#define TOPIC_SOIL_REQUEST_LIST "home/esp32/soil/request-list"