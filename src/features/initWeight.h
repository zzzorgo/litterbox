#ifndef LitterboxWeight
#define LitterboxWeight

#include <HX711.h>

#include "initTime.h"

#include "../pinConfig.h"

#define SENSOR_AMOUNT 4
#define WEIGHT_BUFFER_SIZE 200

using ScaleSensor = HX711;
using SemaphoreHandle = SemaphoreHandle_t;

struct WeightConfig {
    GpioNums clockPin;
    GpioNums dataPin;
    float scale;
};

struct WeightEntry {
    TimeMs time;
    long value; // in centigrams
};

struct Buffer {
    WeightEntry data[WEIGHT_BUFFER_SIZE];
    int position;
};

void weightBegin(const WeightConfig configs[SENSOR_AMOUNT]);
void popWeightBuffer(Buffer outputBuffers[SENSOR_AMOUNT]);

#endif
