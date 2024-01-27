#ifndef LitterboxHttpClient
#define LitterboxHttpClient

#include "initWeight.h"

#define WEIGHT_SENSOR_ID_PREFIX "w"
#define SOFT_STABLE_BUFFER_SIZE 10

int MAX_LENGTH = 50;

int STABLE_THRESHOLD = 20;               // centigrams
int SOFT_STABLE_THRESHOLD = 900;         // centigrams
int ACTION_SENSITIVITY_THRESHOLD = 100;  // centigrams

void sendWeighData(Buffer buffers[SENSOR_AMOUNT]);

#endif
