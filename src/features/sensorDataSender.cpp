#include <Arduino.h>
#include <HTTPClient.h>
#include <cppQueue.h>

#include "sensorDataSender.h"

void httpPostString(String *str, int retryCount = 0, int requestsSent = 0)
{
    HTTPClient http;
    http.begin("http://192.168.178.201:8088/api/sensors/data");
    http.addHeader("Content-Type", "text/CSV");

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

void sendPooCountAndWeight(long pooWeight)
{
    String str;
    str.reserve(270);
    UnixTimeMs nowTime = getUnixTimeMs();

    str += nowTime;
    str += ";poo-count;";
    str += state.pooCount;
    str += '\n';
    str += nowTime;
    str += ";w-poo;";
    str += pooWeight;
    str += '\n';
    str += nowTime;
    str += ";w-cat;";
    str += state.catWeight;
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

        if (prevLitterBoxState != state.litterBoxState)
        {
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
                        sendPooCountAndWeight(vesselDiff);
                    }
                    else if (vesselDiff < -ACTION_SENSITIVITY_THRESHOLD)
                    {
                        state.pooCount = 0;
                        state.rerender = true;
                        sendPooCount();
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

            UnixTimeMs timeOfStateChange = 0;
            if (prevLitterBoxState < state.litterBoxState)
            {
                timeOfStateChange = entry.time;
            } else {
                WeightEntry queueValue;
                timeOfStateChange = sampleQueue.peek(&queueValue);
                timeOfStateChange = queueValue.time;
            }

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

        for (int i = 0; i < SENSOR_AMOUNT; i++)
        {
            sample[j].time += buffers[i].data[j].time;
            sample[j].value += buffers[i].data[j].value;
        }

        sample[j].time = sample[j].time / SENSOR_AMOUNT;

        next(sample[j]);
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
    sendRawData(buffers);
    sendAggregatedData(buffers);
}
