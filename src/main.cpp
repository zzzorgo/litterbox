#include <Arduino.h>
#include <cppQueue.h>

#include "main.h"
#include "report.h"
#include "connectWifi.h"
#include "initTime.h"
#include "initServer.h"

#define UNDEFINED_VALUE -1

const float MOCK_INPUT[] = {5.16, 5.17, 5.18, 5.18, 5.18, 7.4, 10.52, 10.52, 10.52, 5.37, 5.37, 5.37};
int i = 0;

enum LitterBoxState {
  Undefined,
  Ready,
  CatInside,
  Maintenance
};

LitterBoxState litterBoxState = Undefined;
cppQueue prevValues(sizeof(float *), 2, FIFO);
float prevStableValue = UNDEFINED_VALUE;
float prevVesselWeight = UNDEFINED_VALUE;

void setup() {
  Serial.begin(9600);

  if (!reporterBegin()) {
    return;
  }

  printData(&Serial);
  removeAllData();

  wifiBegin();
  timeBegin();
  serverBegin();

  prevVesselWeight = 5.15;
  litterBoxState = Ready;
}

void loop() {
  Serial.print("State: ");
  Serial.println(litterBoxState);
  float currentTotalWeight = MOCK_INPUT[i % 12];

  if (prevValues.isFull()) {
    float prevQueueValue = currentTotalWeight;
    bool valueIsStable = true;

    for (int j = 0; j < prevValues.getCount(); j++) {
      float queueValue = UNDEFINED_VALUE;
      prevValues.peekIdx(&queueValue, j);

      if (prevQueueValue != queueValue) {
        valueIsStable = false;
        break;
      }
    }

    if (valueIsStable) {
      if (prevStableValue != UNDEFINED_VALUE) {
        float diff = currentTotalWeight - prevStableValue;
        Serial.print("New stable value: ");
        Serial.println(currentTotalWeight);
        Serial.print("Diff: ");
        Serial.println(diff);

        if (litterBoxState == Ready && diff > 0) {
          litterBoxState = CatInside;
        }

        if (litterBoxState == CatInside && diff < 0) {
          litterBoxState = Ready;
          float pooWeight = currentTotalWeight - prevVesselWeight;

          if (pooWeight < 0) {
            exit(-1);
          }

          reportWeight(getFormatedTime(), -diff, pooWeight);
          prevVesselWeight = currentTotalWeight;
        }

        if (litterBoxState == Ready && diff < 0) {
          prevVesselWeight = currentTotalWeight;
        }
      }

      prevStableValue = currentTotalWeight;
    }

    prevValues.drop();
  }

  prevValues.push(&currentTotalWeight);
  i++;
  handleClient(printData);
  // todo: remove delay
  delay(1000);
}
