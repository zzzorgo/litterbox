#include <Arduino.h>
#include <cppQueue.h>

#include "main.h"
#include "initFileWorker.h"
#include "initWifi.h"
#include "initTime.h"
#include "initServer.h"
#include "initState.h"
#include "initRender.h"

const float MOCK_INPUT[] = {5.16, 5.17, 5.18, 5.18, 5.18, 7.4, 10.52, 10.52, 10.52, 5.37, 5.37, 5.37};
int i = 0;

cppQueue prevValues(sizeof(float *), 2, FIFO);
float prevStableValue = UNDEFINED_VALUE;
float prevVesselWeight = UNDEFINED_VALUE;

void setup() {
  renderBegin();
  Serial.begin(9600);

  if (!reporterBegin()) {
    return;
  }

  wifiBegin();
  timeBegin();
  serverBegin();

  prevVesselWeight = 5.15;
  state.litterBoxState = Ready;
}

void loop() {
  Serial.print("State: ");
  Serial.println(state.litterBoxState);
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

        if (state.litterBoxState == Ready && diff > 0) {
          state.litterBoxState = CatInside;
        } else if (state.litterBoxState == CatInside && diff < 0) {
          state.litterBoxState = Ready;
          state.pooCount++;
          state.catWeight = -diff;
          float pooWeight = currentTotalWeight - prevVesselWeight;

          if (pooWeight < 0) {
            exit(-1);
          }

          reportWeight(getFormatedTime(), state.catWeight, pooWeight);
          persistState();
          prevVesselWeight = currentTotalWeight;
        } else if (state.litterBoxState == Ready && diff < 0) {
          prevVesselWeight = currentTotalWeight;
          state.pooCount = 0;
          persistState();
        }
      }

      prevStableValue = currentTotalWeight;
    }

    prevValues.drop();
  }

  prevValues.push(&currentTotalWeight);
  i++;

  renderState();
  handleClient(printData);
  // todo: remove delay
  delay(1000);
}
