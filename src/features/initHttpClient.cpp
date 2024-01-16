#include <Arduino.h>
#include <HTTPClient.h>

#include "initHttpClient.h"

void beginHttpClient()
{
}

void sendWeighData(Buffer buffers[SENSOR_AMOUNT])
{
    HTTPClient http;

    http.begin("http://jsonplaceholder.typicode.com/posts"); // Specify destination for HTTP request
    http.addHeader("Content-Type", "text/plain");            // Specify content-type header

    int httpResponseCode = http.POST("POSTING from ESP32"); // Make the request

    if (httpResponseCode > 0)
    { // Check for the returning code

        String payload = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(payload);
    }

    else
    {
        Serial.println("Error on HTTP request");
    }

    http.end(); // Free the resources
}
