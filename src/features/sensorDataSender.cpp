#include <Arduino.h>
#include <HTTPClient.h>
#include <cppQueue.h>

#include "sensorDataSender.h"
#include "sessionClassifier.h"

HTTPClient http;

void httpPostString(String *str, int retryCount = 0, int requestsSent = 0)
{
    if (!http.connected()) {
        http.end();

        http.begin("http://server.lan:8088/api/sensors/data");
        http.addHeader("Content-Type", "text/CSV");
        http.addHeader("Connection", "keep-alive");
        http.setReuse(true);
    }

    int httpResponseCode = http.POST(str->c_str());
    requestsSent++;

    if (httpResponseCode >= 200 && httpResponseCode < 300)
    {
        String payload = http.getString();
        http.end();
        Serial.print("[data sender] response code: ");
        Serial.println(httpResponseCode);
        Serial.print("[data sender] body: ");
        Serial.println(payload);
    }
    else
    {
        http.end();
        Serial.print("[data sender] Error on HTTP request: ");
        Serial.println(httpResponseCode);

        if (requestsSent <= retryCount) {
            delay(100);
            httpPostString(str, retryCount, requestsSent);
        }
    }
}

void sendRawData(Buffer buffers[SENSOR_AMOUNT])
{ // Specify content-type header
    String str;
    str.reserve(24 * 1024);

    for (int i = 0; i < SENSOR_AMOUNT; i++)
    {
        for (int j = 0; j < buffers[i].position; j++)
        {
            str += buffers[i].data[j].time;
            str += ';';
            str += WEIGHT_SENSOR_ID_PREFIX;
            str += i;
            str += ';';
            str += buffers[i].data[j].value;
            str += '\n';
        }
    }

    httpPostString(&str);
}

void sendSessionResult(
    long outputWeight,
    const SessionClassificationResult &sessionResult
)
{
    String str;
    str.reserve(720);
    UnixTimeMs nowTime = getUnixTimeMs();

    str += nowTime;
    str += ";poo-count;";
    str += state.pooCount;
    str += '\n';
    str += nowTime;
    str += ";w-poo;";
    str += outputWeight;
    str += '\n';
    str += nowTime;
    str += ";w-cat;";
    str += state.catWeight;
    str += '\n';

    // All session fields share the start timestamp, allowing the backend to
    // pivot them into one record and Grafana to render start/end as a range.
    str += sessionResult.startTime;
    str += ";session-start;";
    str += sessionResult.startTime;
    str += '\n';
    str += sessionResult.startTime;
    str += ";session-end;";
    str += sessionResult.endTime;
    str += '\n';
    str += sessionResult.startTime;
    str += ";session-output-weight;";
    str += outputWeight;
    str += '\n';
    str += sessionResult.startTime;
    str += ";session-classification;";
    str += sessionResult.classification;
    str += '\n';
    str += sessionResult.startTime;
    str += ";session-movement-score;";
    str += sessionResult.movementScore;
    str += '\n';
    str += sessionResult.startTime;
    str += ";session-movement-samples;";
    str += sessionResult.movementSampleCount;
    str += '\n';

    httpPostString(&str, 5);
}

void sendPooCount()
{
    String str;
    str.reserve(90);
    UnixTimeMs nowTime = getUnixTimeMs();

    str += nowTime;
    str += ";poo-count;";
    str += state.pooCount;
    str += '\n';

    httpPostString(&str, 5);
}

void sendLitterBoxState(UnixTimeMs ms)
{
    String str;
    str.reserve(90);

    str += ms;
    str += ";state;";
    str += state.litterBoxState;
    str += '\n';

    httpPostString(&str, 5);
}

void reportResetReason()
{
    String str;
    str.reserve(90);
    UnixTimeMs nowTime = getUnixTimeMs();

    str += nowTime;
    str += ";rst-reason;";
    str += esp_reset_reason();
    str += '\n';

    httpPostString(&str);
}

int MAX_LENGTH = 50;
cppQueue sampleQueue(sizeof(WeightEntry), MAX_LENGTH, FIFO);
long prevVesselWeight = UNDEFINED_VALUE;
LitterBoxState prevLitterBoxState = InitialWeight;
long softStableValues[SOFT_STABLE_BUFFER_SIZE] = {};
int softStableValuesCount = 0;
UnixTimeMs lastStateChangeTime = UNDEFINED_VALUE;

void next(WeightEntry entry)
{
    if (sampleQueue.getCount() == MAX_LENGTH)
    {
        long maxDiff = 0;

        for (int j = 0; j < sampleQueue.getCount() - 1; j++)
        {
            WeightEntry queueValue;
            WeightEntry nextQueueValue;

            sampleQueue.peekIdx(&queueValue, j);
            sampleQueue.peekIdx(&nextQueueValue, j + 1);

            long diff = abs(queueValue.value - nextQueueValue.value);

            if (maxDiff < diff)
            {
                maxDiff = diff;
            }
        }

        long sum = 0;

        for (int j = 0; j < sampleQueue.getCount(); j++)
        {
            WeightEntry queueValue;
            sampleQueue.peekIdx(&queueValue, j);

            sum += queueValue.value;
        }

        long average = sum / sampleQueue.getCount();

        if (maxDiff < STABLE_THRESHOLD)
        {
            state.litterBoxState = StableWeight;
        }
        else if (maxDiff >= STABLE_THRESHOLD && maxDiff < SOFT_STABLE_THRESHOLD)
        {
            state.litterBoxState = SoftStableWeight;
        }
        else if (maxDiff >= SOFT_STABLE_THRESHOLD)
        {
            state.litterBoxState = UnstableWeight;
        }

        if (prevLitterBoxState != state.litterBoxState && prevVesselWeight != UNDEFINED_VALUE)
        {
            UnixTimeMs timeOfStateChange = 0;
            if (prevLitterBoxState < state.litterBoxState)
            {
                timeOfStateChange = entry.time;
            } else {
                WeightEntry queueValue;
                timeOfStateChange = sampleQueue.peek(&queueValue);
                timeOfStateChange = max(queueValue.time, lastStateChangeTime + 1);
            }

            if (
                prevLitterBoxState == StableWeight &&
                state.litterBoxState != StableWeight
            )
            {
                startSessionClassification(timeOfStateChange);
            }

            if (state.litterBoxState == StableWeight)
            {
                if (prevLitterBoxState != InitialWeight)
                {
                    long vesselDiff = average - prevVesselWeight;

                    if (vesselDiff > ACTION_SENSITIVITY_THRESHOLD)
                    {
                        long maxSoftStableValue = 0;

                        for (int i = 0; i < softStableValuesCount; i++)
                        {
                            if (maxSoftStableValue < softStableValues[i])
                            {
                                maxSoftStableValue = softStableValues[i];
                            }
                        }

                        long catWeight = maxSoftStableValue - average;
                        state.pooCount++;
                        state.catWeight = catWeight;
                        state.rerender = true;
                        SessionClassificationResult sessionResult =
                            finishSessionClassification(timeOfStateChange);
                        sendSessionResult(vesselDiff, sessionResult);
                    }
                    else if (vesselDiff < -ACTION_SENSITIVITY_THRESHOLD)
                    {
                        state.pooCount = 0;
                        state.rerender = true;
                        cancelSessionClassification();
                        sendPooCount();
                    }
                    else
                    {
                        cancelSessionClassification();
                    }

                    softStableValuesCount = 0;
                }
            }

            if (state.litterBoxState == SoftStableWeight)
            {
                softStableValues[softStableValuesCount] = average;
                softStableValuesCount++;

                if (softStableValuesCount >= SOFT_STABLE_BUFFER_SIZE)
                {
                    Serial.println("[data sender] softStableValues overflow");
                    softStableValuesCount = 0;
                }
            }

            lastStateChangeTime = timeOfStateChange;
            sendLitterBoxState(timeOfStateChange);
            prevLitterBoxState = state.litterBoxState;
        }

        if (state.litterBoxState == StableWeight)
        {
            prevVesselWeight = average;
        }

        sampleQueue.drop();
    }

    sampleQueue.push(&entry);
}

void sendAggregatedData(Buffer buffers[SENSOR_AMOUNT])
{
    int sampleLength = WEIGHT_BUFFER_SIZE;

    for (int i = 0; i < SENSOR_AMOUNT; i++)
    {
        if (buffers[i].position < sampleLength)
        {
            sampleLength = buffers[i].position;
        }
    }

    WeightEntry sample[sampleLength];

    for (int j = 0; j < sampleLength; j++)
    {
        sample[j] = {.time = 0, .value = 0};
        long sensorValues[SENSOR_AMOUNT];

        for (int i = 0; i < SENSOR_AMOUNT; i++)
        {
            sample[j].time += buffers[i].data[j].time;
            sample[j].value += buffers[i].data[j].value;
            sensorValues[i] = buffers[i].data[j].value;
        }

        sample[j].time = sample[j].time / SENSOR_AMOUNT;

        next(sample[j]);
        addSessionClassificationSample(sensorValues, state.litterBoxState);
    }

    String str;
    str.reserve(6 * 1024);

    for (int j = 0; j < sampleLength; j++)
    {
        str += sample[j].time;
        str += ";w-sum;";
        str += sample[j].value;
        str += '\n';
    }

    httpPostString(&str);
}

void sendWeighData(Buffer buffers[SENSOR_AMOUNT])
{
    sendAggregatedData(buffers);
    sendRawData(buffers);
}
