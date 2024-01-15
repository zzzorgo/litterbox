#ifndef LitterboxWeight
#define LitterboxWeight

#include <HX711.h>
#include "../pinConfig.h"

#define SENSOR_AMOUNT 4
using ScaleSensor = HX711;

struct WeightConfig {
    GpioNums clockPin;
    GpioNums dataPin;
};

void weightBegin(const WeightConfig configs[SENSOR_AMOUNT]);

#endif
