#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

const char FILE_PATH[] = "/data.csv";

bool reportWeight(const String& dateString, float catWeight, float pooWeight) {
  File file = SPIFFS.open(FILE_PATH, FILE_APPEND);

  if(!file){
    Serial.println("[reporter] Failed to open file");

    return false;
  }

  file.print(dateString);
  file.print(";");
  file.print(catWeight);
  file.print(";");
  file.println(pooWeight);

  Serial.print("[reporter] Total: ");
  Serial.print(SPIFFS.totalBytes());
  Serial.println(" bytes");

  Serial.print("[reporter] Used: ");
  Serial.print(SPIFFS.usedBytes());
  Serial.println(" bytes");

  file.close();

  return true;
}

bool reporterBegin() {
  if (!SPIFFS.begin(true)) {
    Serial.println("[reporter] Failed to initialize FS");

    return false;
  }

  return true;
}

bool removeAllData() {
  if (SPIFFS.exists(FILE_PATH)) {
    return SPIFFS.remove(FILE_PATH);
  } else {
    return true;
  }
}

bool printData(Print* printTarget) {
  File file = SPIFFS.open(FILE_PATH);

  if(!file){
    Serial.println("[reporter] Failed to open file for reading");
    return false;
  }

  while(file.available()) {
    printTarget->write(file.read());
  }

  file.close();

  return true;
}
