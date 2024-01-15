#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include "initState.h"

const char *ssid = "PunksNotDead2";
const char *password = "Ckj;ysq0gfhjkm";

void wifiBegin() {
  WiFi.begin(ssid, password);

  // todo: retry on timeout?
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("[Wifi] Connecting...");
  }

  Serial.println("[Wifi] Connected!");
  state.ip = WiFi.localIP();
}

void wifiSleep() {
  esp_wifi_stop();
}
