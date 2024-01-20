#include <Arduino.h>
#include <HTTPClient.h>
// #include <cppQueue.h>

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
        Serial.print("[http client] response code: ");
        Serial.println(httpResponseCode);
        Serial.print("[http client] body: ");
        Serial.println(payload);
    }

    else
    {
        Serial.print("[http client] Error on HTTP request: ");
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

// cppQueue prevValues(sizeof(WeightEntry *), 2, FIFO);

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
