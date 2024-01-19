#include <Arduino.h>
#include <HTTPClient.h>

#include "initHttpClient.h"

void beginHttpClient()
{
}

void sendWeighData(Buffer buffers[SENSOR_AMOUNT])
{
    HTTPClient http;

    http.begin("http://192.168.178.201:8088/api/sensors/data"); // Specify destination for HTTP request
    http.addHeader("Content-Type", "text/CSV");            // Specify content-type header

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

    int httpResponseCode = http.POST(str.c_str()); // Make the request

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
