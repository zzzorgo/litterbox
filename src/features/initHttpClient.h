#ifndef LitterboxHttpClient
#define LitterboxHttpClient

#include "initWeight.h"

#define WEIGHT_SENSOR_ID_PREFIX "w"

void sendWeighData(Buffer buffers[SENSOR_AMOUNT]);

#endif
