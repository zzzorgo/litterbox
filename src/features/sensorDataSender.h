#ifndef LitterboxHttpClient
#define LitterboxHttpClient

#include "initWeight.h"

#define WEIGHT_SENSOR_ID_PREFIX "w"
#define STABILITY_THRESHOLD 250 // centigrams

void sendWeighData(Buffer buffers[SENSOR_AMOUNT]);

#endif
