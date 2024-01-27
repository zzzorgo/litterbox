#include <Arduino.h>
#include <HTTPClient.h>
#include <cppQueue.h>

#include "sensorDataSender.h"

void httpPostString(String *str)
{
    HTTPClient http;

    http.begin("http://192.168.178.201:8088/api/sensors/data"); // Specify destination for HTTP request
    http.addHeader("Content-Type", "text/CSV");

    int httpResponseCode = http.POST(str->c_str()); // Make the request

    if (httpResponseCode > 0)
    { // Check for the returning code
        String payload = http.getString();
        Serial.print("[data sender] response code: ");
        Serial.println(httpResponseCode);
        Serial.print("[data sender] body: ");
        Serial.println(payload);
    }

    else
    {
        Serial.print("[data sender] Error on HTTP request: ");
        Serial.println(httpResponseCode);
    }

    http.end(); // Free the resources
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

void sendPooCountAndWeight(long pooWeight) {
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

    httpPostString(&str);
}

cppQueue sampleQueue(sizeof(long *), 200, FIFO);
long prevVesselWeight = UNDEFINED_VALUE;

void next(long *value)
{
    long sum = 0;

    for (int j = 0; j < sampleQueue.getCount(); j++) {
        long queueValue = UNDEFINED_VALUE;
        sampleQueue.peekIdx(&queueValue, j);

        sum += queueValue;
    }

    long average = UNDEFINED_VALUE;

    if (sampleQueue.getCount() == 0) {
        long average = *value;
    } else {
        average = sum / sampleQueue.getCount();
    }

    long diff = *value - average;

    if (diff > 250 && state.litterBoxState == Ready) {
        prevVesselWeight = average;
        state.litterBoxState = Rising;
    }

    if (abs(diff) < 20 && state.litterBoxState == Rising) {
        state.catWeight = average - prevVesselWeight;
        state.litterBoxState = StableHigh;
    }

    if (-diff > 2000 && state.litterBoxState == StableHigh) {
        state.litterBoxState = Falling;
    }

    if (abs(diff) < 20 && state.litterBoxState == Falling) {
        long pooWeight = *value - prevVesselWeight;
        prevVesselWeight = *value;
        state.pooCount += 1;
        state.litterBoxState = Ready;

        sendPooCountAndWeight(pooWeight);
    }

    if (sampleQueue.isFull())
    {
        sampleQueue.drop();
    }

    sampleQueue.push(value);
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

        next(&sample[j].value);
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
    // Some times data is collected before the time is initialized even though
    // ntp server is configured before that moment. A proper fix is to sync
    // ntp server request with this function
    if (buffers[0].data[0].time < 1705754118000)
    {
        Serial.println("[data sender] invalid time, sending data aborted");

        return;
    }

    sendRawData(buffers);
    sendAggregatedData(buffers);
}
