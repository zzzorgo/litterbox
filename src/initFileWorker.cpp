#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

#include "initState.h"

const char DATA_FILE_PATH[] = "/data.csv";
const char STATE_FILE_PATH[] = "/state.txt";

void reportFlashMemoryUsage() {
  Serial.print("[reporter] Total: ");
  Serial.print(SPIFFS.totalBytes());
  Serial.println(" bytes");

  Serial.print("[reporter] Used: ");
  Serial.print(SPIFFS.usedBytes());
  Serial.println(" bytes");
}

bool reportWeight(const String& dateString, float catWeight, float pooWeight) {
  File file = SPIFFS.open(DATA_FILE_PATH, FILE_APPEND);

  if(!file){
    Serial.print("[reporter] Failed to open file ");
    Serial.println(DATA_FILE_PATH);

    return false;
  }

  file.print(dateString);
  file.print(";");
  file.print(catWeight);
  file.print(";");
  file.println(pooWeight);

  reportFlashMemoryUsage();

  file.close();

  return true;
}

bool persistState() {
  File file = SPIFFS.open(STATE_FILE_PATH, FILE_WRITE);

  if(!file){
    Serial.print("[reporter] Failed to open file ");
    Serial.println(STATE_FILE_PATH);

    return false;
  }

  file.println(state.pooCount);
  file.println(state.catWeight);

  reportFlashMemoryUsage();

  file.close();

  return true;
}

bool restoreState() {
  File file = SPIFFS.open(STATE_FILE_PATH);

  if(!file){
    Serial.print("[reporter] Failed to open file for reading ");
    Serial.println(STATE_FILE_PATH);
    return false;
  }

  state.pooCount = file.readStringUntil('\n').toInt();
  state.catWeight = file.readStringUntil('\n').toFloat();

  file.close();

  return true;
}

bool reporterBegin() {
  if (!SPIFFS.begin(true)) {
    Serial.println("[reporter] Failed to initialize FS");

    return false;
  }

  restoreState();

  return true;
}

bool removeAllData() {
  if (SPIFFS.exists(DATA_FILE_PATH)) {
    return SPIFFS.remove(DATA_FILE_PATH);
  } else {
    return true;
  }
}

bool printData(Print* printTarget) {
  File file = SPIFFS.open(DATA_FILE_PATH);

  if(!file){
    Serial.println("[reporter] Failed to open file for reading");
    return false;
  }

  while(file.available()) {
    printTarget->write(file.read());
  }

  file.close();
  removeAllData();

  return true;
}
