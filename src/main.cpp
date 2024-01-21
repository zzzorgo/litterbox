#include <Arduino.h>
#include <cppQueue.h>

#include "./features/initFileWorker.h"
#include "./features/initRender.h"
#include "./features/initServer.h"
#include "./features/initSleep.h"
#include "./features/initState.h"
#include "./features/initTime.h"
#include "./features/initWeight.h"
#include "./features/initWifi.h"
#include "./features/sensorDataSender.h"

#include "main.h"
#include "pinConfig.h"

// scale.set_scale(400); // 15 2
// scale.set_scale(462.83186); // 13 12
// scale.set_scale(422.56637); // 4 16
// scale.set_scale(415.92920); // 14 27

// scale gives value in centigrams
WeightConfig weightConfigs[SENSOR_AMOUNT] = {
  {.clockPin = weightSensorClockPin1, .dataPin = weightSensorDataPin1, .scale = 40.0},
  {.clockPin = weightSensorClockPin2, .dataPin = weightSensorDataPin2, .scale = 42.256637},
  {.clockPin = weightSensorClockPin3, .dataPin = weightSensorDataPin3, .scale = 46.283186},
  {.clockPin = weightSensorClockPin4, .dataPin = weightSensorDataPin4, .scale = 41.592920},
};

Buffer tempBuffers[SENSOR_AMOUNT] = {
  {.data = {}, .position = 0},
  {.data = {}, .position = 0},
  {.data = {}, .position = 0},
  {.data = {}, .position = 0},
};

void setup() {
  sleepBegin(buttonPin);
  renderBegin(buttonPin);

  Serial.begin(115200);

  if (!reporterBegin()) {
    return;
  }

  wifiBegin();
  timeBegin();
  serverBegin();

  weightBegin(weightConfigs);

  state.litterBoxState = Ready;

  persistState();
}

void loop() {
  renderState();
  handleClient(printData);
  // todo: remove delay?
  delay(3000);

  popWeightBuffer(&tempBuffers[0]);
  sendWeighData(&tempBuffers[0]);

  persistState();

  // sleepOnTimeout();
}
