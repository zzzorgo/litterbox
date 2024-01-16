#include <HX711.h>
#include <cppQueue.h>

#include "initWeight.h"

ScaleSensor scaleSensors[SENSOR_AMOUNT];
bool needResetZero[SENSOR_AMOUNT] = {true, true, true, true};

Buffer buffers[] = {
    {.data = {}, .position = 0},
    {.data = {}, .position = 0},
    {.data = {}, .position = 0},
    {.data = {}, .position = 0},
};

xSemaphoreHandle mutexes[SENSOR_AMOUNT] = {
    xSemaphoreCreateMutex(),
    xSemaphoreCreateMutex(),
    xSemaphoreCreateMutex(),
    xSemaphoreCreateMutex(),
};

void weightingTask(void *parameter)
{
    int *sensorNumberPointer = (int *)parameter;
    int sensorNumber = *sensorNumberPointer;
    ScaleSensor scaleSensor = scaleSensors[sensorNumber];

    while (true)
    {
        if (scaleSensor.wait_ready_timeout(1000))
        {
            long reading = scaleSensor.get_units();

            xSemaphoreTake(mutexes[sensorNumber], portMAX_DELAY);
            buffers[sensorNumber].data[buffers[sensorNumber].position] = {.time = getUnixTime(), .value = reading};
            buffers[sensorNumber].position += 1;
            xSemaphoreGive(mutexes[sensorNumber]);
        } else {
            Serial.print("Sensor not found: ");
            Serial.println(sensorNumber);
        }

        taskYIELD();
    }
}

void weightBegin(const WeightConfig configs[SENSOR_AMOUNT])
{
    for (int i = 0; i < SENSOR_AMOUNT; i++)
    {
        WeightConfig config = configs[i];
        scaleSensors[i].begin(config.dataPin, config.clockPin);
        int* sensorNumber = new int(i);

        if (needResetZero[i])
        {
            ScaleSensor scaleSensor = scaleSensors[i];
            scaleSensor.tare(10);
            scaleSensor.set_scale(41.77);
            needResetZero[i] = false;
        }

        Serial.println(*sensorNumber);

        xTaskCreate(
            weightingTask,
            "weightingTask" + i,
            1500,
            sensorNumber,
            tskIDLE_PRIORITY,
            NULL /* Task handle. */
        );
    }
}

void popWeightBuffer(Buffer outputBuffers[SENSOR_AMOUNT]) {
    for (int i = 0; i < SENSOR_AMOUNT; i++)
    {
        xSemaphoreTake(mutexes[i], portMAX_DELAY);
    }

    for (int i = 0; i < SENSOR_AMOUNT; i++)
    {
        int bufferSize = buffers[i].position;
        Serial.print("size: ");
        Serial.println(bufferSize);
        memcpy(outputBuffers, &buffers, sizeof(buffers));
        buffers[i].position = 0;
    }

    for (int i = 0; i < SENSOR_AMOUNT; i++)
    {
        xSemaphoreGive(mutexes[i]);
    }
}