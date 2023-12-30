#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>

WiFiServer server(80);

void serverBegin() {
  server.begin();
  MDNS.addService("http", "tcp", 80);
  Serial.println("[server] Started!");
}

void handleClient(bool (*pushData)(Print*)) {
  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  // todo: interruption?
  while(client.connected() && !client.available()) {
    delay(1);
  }

  String rawRequest = client.readStringUntil('\r');

  pushData(&client);

  client.stop();
}
