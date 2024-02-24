#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

#include "initState.h"
#include "initFileWorker.h"

const char DATA_FILE_PATH[] = "/data.csv";
const char STATE_FILE_PATH[] = "/state.txt";

void reportFlashMemoryUsage()
{
  Serial.print("[reporter] Total: ");
  Serial.print(SPIFFS.totalBytes());
  Serial.println(" bytes");

  Serial.print("[reporter] Used: ");
  Serial.print(SPIFFS.usedBytes());
  Serial.println(" bytes");
}

bool reportWeight(UnixTime ms, int catWeight, float pooWeight)
{
  File file = SPIFFS.open(DATA_FILE_PATH, FILE_APPEND);

  if (!file)
  {
    Serial.print("[reporter] Failed to open file ");
    Serial.println(DATA_FILE_PATH);

    return false;
  }

  file.print(ms);
  file.print(";");
  file.print(catWeight);
  file.print(";");
  file.println(pooWeight);

  reportFlashMemoryUsage();

  file.close();

  return true;
}

State prevState;

bool persistState()
{
  if (
    // true
    state.catWeight != prevState.catWeight ||
    state.pooCount != prevState.pooCount ||
    state.offsets[0] != prevState.offsets[0] ||
    state.offsets[1] != prevState.offsets[1] ||
    state.offsets[2] != prevState.offsets[2] ||
    state.offsets[3] != prevState.offsets[3]
  )
  {
    // state.pooCount = 0;
    // state.catWeight = 52370;
    File file = SPIFFS.open(STATE_FILE_PATH, FILE_WRITE);

    if (!file)
    {
      Serial.print("[reporter] Failed to open file ");
      Serial.println(STATE_FILE_PATH);

      return false;
    }

    file.println(state.pooCount);
    file.println(state.catWeight);

    file.println(state.offsets[0]);
    file.println(state.offsets[1]);
    file.println(state.offsets[2]);
    file.println(state.offsets[3]);

    reportFlashMemoryUsage();

    file.close();

    prevState.catWeight = state.catWeight;
    prevState.pooCount = state.pooCount;
    prevState.offsets[0] = state.offsets[0];
    prevState.offsets[1] = state.offsets[1];
    prevState.offsets[2] = state.offsets[2];
    prevState.offsets[3] = state.offsets[3];
  }

  return true;
}

bool restoreState()
{
  File file = SPIFFS.open(STATE_FILE_PATH);

  if (!file)
  {
    Serial.print("[reporter] Failed to open file for reading ");
    Serial.println(STATE_FILE_PATH);
    return false;
  }

  state.pooCount = file.readStringUntil('\n').toInt();
  state.catWeight = file.readStringUntil('\n').toInt();

  state.offsets[0] = file.readStringUntil('\n').toInt();
  state.offsets[1] = file.readStringUntil('\n').toInt();
  state.offsets[2] = file.readStringUntil('\n').toInt();
  state.offsets[3] = file.readStringUntil('\n').toInt();

  file.close();

  return true;
}

bool reporterBegin()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("[reporter] Failed to initialize FS");

    return false;
  }

  restoreState();

  return true;
}

bool removeAllData()
{
  if (SPIFFS.exists(DATA_FILE_PATH))
  {
    return SPIFFS.remove(DATA_FILE_PATH);
  }
  else
  {
    return true;
  }
}

bool printData(Print *printTarget)
{
  File file = SPIFFS.open(DATA_FILE_PATH);

  if (!file)
  {
    Serial.println("[reporter] Failed to open file for reading");
    return false;
  }

  while (file.available())
  {
    printTarget->write(file.read());
  }

  file.close();
  removeAllData();

  return true;
}
