#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "PunksNotDead2";
const char *password = "Ckj;ysq0gfhjkm";

void wifiBegin() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("[Wifi] Connecting...");
  }

  Serial.println("[Wifi] Connected!");
}
