#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include "initState.h"
#include "../secrets.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

void wifiBegin() {
  WiFi.begin(ssid, password);

  // todo: retry on timeout?
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("[Wifi] Connecting...");
  }

  Serial.println("[Wifi] Connected!");
  state.ip = WiFi.localIP();
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void wifiSleep() {
  esp_wifi_stop();
}
