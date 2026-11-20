#include "sessionClassifier.h"

// These values are in the scale's current units. The thresholds were selected
// from 51 manually labelled October/November 2026 sessions. A median is used
// because occasional large sensor deviations distorted the arithmetic mean.
#define SENSOR_MOVEMENT_NOISE_FLOOR 10
#define PEE_MOVEMENT_SCORE_THRESHOLD 100
#define POO_MOVEMENT_SCORE_THRESHOLD 125
#define MIN_MOVEMENT_SAMPLE_COUNT 50
#define MOVEMENT_HISTOGRAM_BUCKET_WIDTH 5
#define MOVEMENT_HISTOGRAM_BUCKET_COUNT 128

struct SessionClassifierState {
    bool active;
    bool hasPreviousValues;
    UnixTimeMs startTime;
    unsigned long movementSampleCount;
    long previousSensorValues[SENSOR_AMOUNT];
    unsigned short movementHistogram[MOVEMENT_HISTOGRAM_BUCKET_COUNT];
};

SessionClassifierState classifierState = {};

void startSessionClassification(UnixTimeMs startTime)
{
    classifierState.active = true;
    classifierState.hasPreviousValues = false;
    classifierState.startTime = startTime;
    classifierState.movementSampleCount = 0;

    for (int i = 0; i < MOVEMENT_HISTOGRAM_BUCKET_COUNT; i++)
    {
        classifierState.movementHistogram[i] = 0;
    }
}

void addSessionClassificationSample(
    const long sensorValues[SENSOR_AMOUNT],
    LitterBoxState litterBoxState
)
{
    if (!classifierState.active)
    {
        return;
    }

    if (classifierState.hasPreviousValues && litterBoxState == SoftStableWeight)
    {
        long movement = 0;

        for (int i = 0; i < SENSOR_AMOUNT; i++)
        {
            long difference = sensorValues[i] - classifierState.previousSensorValues[i];
            if (difference < 0)
            {
                difference = -difference;
            }

            if (difference > SENSOR_MOVEMENT_NOISE_FLOOR)
            {
                movement += difference - SENSOR_MOVEMENT_NOISE_FLOOR;
            }
        }

        int bucket = movement / MOVEMENT_HISTOGRAM_BUCKET_WIDTH;
        if (bucket >= MOVEMENT_HISTOGRAM_BUCKET_COUNT)
        {
            bucket = MOVEMENT_HISTOGRAM_BUCKET_COUNT - 1;
        }

        classifierState.movementHistogram[bucket]++;
        classifierState.movementSampleCount++;
    }

    for (int i = 0; i < SENSOR_AMOUNT; i++)
    {
        classifierState.previousSensorValues[i] = sensorValues[i];
    }
    classifierState.hasPreviousValues = true;
}

SessionClassificationResult finishSessionClassification(UnixTimeMs endTime)
{
    SessionClassificationResult result = {
        .startTime = classifierState.startTime,
        .endTime = endTime,
        .classification = SessionUncertain,
        .movementScore = 0,
        .movementSampleCount = classifierState.movementSampleCount,
    };

    if (classifierState.movementSampleCount > 0)
    {
        unsigned long medianPosition = (classifierState.movementSampleCount - 1) / 2;
        unsigned long samplesSeen = 0;

        for (int bucket = 0; bucket < MOVEMENT_HISTOGRAM_BUCKET_COUNT; bucket++)
        {
            samplesSeen += classifierState.movementHistogram[bucket];
            if (samplesSeen > medianPosition)
            {
                result.movementScore = bucket * MOVEMENT_HISTOGRAM_BUCKET_WIDTH;
                break;
            }
        }
    }

    if (classifierState.movementSampleCount >= MIN_MOVEMENT_SAMPLE_COUNT)
    {
        if (result.movementScore <= PEE_MOVEMENT_SCORE_THRESHOLD)
        {
            result.classification = SessionPee;
        }
        else if (result.movementScore >= POO_MOVEMENT_SCORE_THRESHOLD)
        {
            result.classification = SessionPoo;
        }
    }

    cancelSessionClassification();
    return result;
}

void cancelSessionClassification()
{
    classifierState.active = false;
    classifierState.hasPreviousValues = false;
    classifierState.startTime = 0;
    classifierState.movementSampleCount = 0;
}
