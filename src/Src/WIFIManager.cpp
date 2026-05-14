#include "inc/WIFIManager.h"

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

#include "inc/config.h"

static WebServer server(80);
static Preferences prefs;

void WIFIManager::begin() {
  pinMode(WIFI_SETUP_BUTTON_PIN, INPUT_PULLUP);

  connectSavedWiFi();

  if (!isConnected()) {
    startSetupPortal();
  }
}

void WIFIManager::handle() {
  if (setupMode) {
    server.handleClient();
    return;
  }

  handleSetupButton();
}

bool WIFIManager::isSetupMode() {
  return setupMode;
}

bool WIFIManager::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void WIFIManager::connectSavedWiFi() {
  prefs.begin("wifi", true);

  String ssid = prefs.getString("ssid", "");
  String password = prefs.getString("password", "");

  prefs.end();

  if (ssid.length() == 0) {
    Serial.println("WiFi credentials not found");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.print("Connecting to WiFi");

  unsigned long startedAt = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startedAt < WIFI_CONNECT_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed");
  }
}

void WIFIManager::startSetupPortal() {
  setupMode = true;

  WiFi.disconnect(true);
  delay(300);

  WiFi.mode(WIFI_AP);

  String apName = "HAUS-Setup-" + String(DEVICE_LOGIN);
  WiFi.softAP(apName.c_str());

  server.on("/", HTTP_GET, [this]() {
    server.send(200, "text/html", buildPage());
  });

  server.on("/save", HTTP_POST, []() {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    prefs.begin("wifi", false);
    prefs.putString("ssid", ssid);
    prefs.putString("password", password);
    prefs.end();

    server.send(
      200,
      "text/html",
      "<html><body><h2>Saved. ESP32 restarting...</h2></body></html>"
    );

    delay(1500);
    ESP.restart();
  });

  server.begin();

  Serial.println("WiFi setup portal started");
  Serial.print("AP name: ");
  Serial.println(apName);
  Serial.print("Open: ");
  Serial.println(WiFi.softAPIP());
}

void WIFIManager::handleSetupButton() {
  if (digitalRead(WIFI_SETUP_BUTTON_PIN) == LOW) {
    if (buttonPressedAt == 0) {
      buttonPressedAt = millis();
    }

    if (millis() - buttonPressedAt >= WIFI_SETUP_HOLD_MS) {
      Serial.println("WiFi setup button held. Starting portal...");

      prefs.begin("wifi", false);
      prefs.remove("ssid");
      prefs.remove("password");
      prefs.end();

      startSetupPortal();
      buttonPressedAt = 0;
    }
  } else {
    buttonPressedAt = 0;
  }
}

String WIFIManager::buildPage() {
  return String(R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
  <meta charset="UTF-8">
  <title>HAUS WiFi Setup</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">

  <style>
    body {
      margin: 0;
      font-family: Arial, sans-serif;
      background: #f3f4f6;
      padding: 24px;
    }

    .card {
      max-width: 420px;
      margin: 40px auto;
      background: white;
      border-radius: 18px;
      padding: 24px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.08);
    }

    h2 {
      margin-top: 0;
    }

    .info {
      background: #f1f5f9;
      border-radius: 12px;
      padding: 14px;
      margin-bottom: 18px;
      line-height: 1.6;
    }

    label {
      display: block;
      margin-bottom: 6px;
      font-weight: bold;
    }

    input {
      width: 100%;
      padding: 12px;
      margin-bottom: 16px;
      border-radius: 10px;
      border: 1px solid #d1d5db;
      box-sizing: border-box;
      font-size: 16px;
    }

    button {
      width: 100%;
      padding: 12px;
      border: 0;
      border-radius: 10px;
      background: #111827;
      color: white;
      font-size: 16px;
    }
  </style>
</head>

<body>
  <div class="card">
    <h2>Настройка Wi-Fi</h2>

    <div class="info">
      <div><b>Логин ESP32:</b> )rawliteral") + String(DEVICE_LOGIN) + R"rawliteral(</div>
      <div><b>Пароль ESP32:</b> )rawliteral" + String(DEVICE_PASSWORD) + R"rawliteral(</div>
    </div>

    <form method="POST" action="/save">
      <label>Имя Wi-Fi</label>
      <input name="ssid" required>

      <label>Пароль Wi-Fi</label>
      <input name="password" type="password">

      <button type="submit">Сохранить и подключиться</button>
    </form>
  </div>
</body>
</html>
)rawliteral";
}