#ifndef LitterboxHttpClient
#define LitterboxHttpClient

#include "initWeight.h"

#define WEIGHT_SENSOR_ID_PREFIX "w"
#define SOFT_STABLE_BUFFER_SIZE 10

#define STABLE_THRESHOLD 20               // centigrams
#define SOFT_STABLE_THRESHOLD 900         // centigrams
#define ACTION_SENSITIVITY_THRESHOLD 100  // centigrams

void sendWeighData(Buffer buffers[SENSOR_AMOUNT]);
void reportResetReason();

#endif
