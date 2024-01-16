#ifndef LitterboxWeight
#define LitterboxWeight

#include <HX711.h>

#include "initTime.h"

#include "../pinConfig.h"

#define SENSOR_AMOUNT 4
using ScaleSensor = HX711;
using SemaphoreHandle = SemaphoreHandle_t;

struct WeightConfig {
    GpioNums clockPin;
    GpioNums dataPin;
};

struct WeightEntry {
    TimeMs time;
    int value; // in centigrams
};

struct Buffer {
    WeightEntry data[200];
    int position;
};

void weightBegin(const WeightConfig configs[SENSOR_AMOUNT]);
void popWeightBuffer(Buffer outputBuffers[SENSOR_AMOUNT]);

#endif
