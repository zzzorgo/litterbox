#ifndef LitterboxSessionClassifier
#define LitterboxSessionClassifier

#include "initState.h"
#include "initTime.h"
#include "initWeight.h"

enum SessionClassification {
    SessionUncertain = 0,
    SessionPee = 1,
    SessionPoo = 2,
};

struct SessionClassificationResult {
    UnixTimeMs startTime;
    UnixTimeMs endTime;
    SessionClassification classification;
    long movementScore;
    unsigned long movementSampleCount;
};

void startSessionClassification(UnixTimeMs startTime);
void addSessionClassificationSample(
    const long sensorValues[SENSOR_AMOUNT],
    LitterBoxState litterBoxState
);
SessionClassificationResult finishSessionClassification(UnixTimeMs endTime);
void cancelSessionClassification();

#endif
