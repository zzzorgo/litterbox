#include <HX711.h>
#include <cppQueue.h>

#include "initWeight.h"

ScaleSensor scaleSensors[SENSOR_AMOUNT];

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

    Serial.print("[weight] Sensor ");
    Serial.print(sensorNumber);
    Serial.println(" started");

    while (true)
    {
        if (scaleSensor.wait_ready_timeout(1000))
        {
            long reading = scaleSensor.get_units();

            xSemaphoreTake(mutexes[sensorNumber], portMAX_DELAY);
            buffers[sensorNumber].data[buffers[sensorNumber].position] = {.time = getUnixTimeMs(), .value = reading};
            buffers[sensorNumber].position += 1;

            if (buffers[sensorNumber].position >= WEIGHT_BUFFER_SIZE) {
                Serial.println("[weight] buffer overflow!");
                buffers[sensorNumber].position = 0;
            }

            xSemaphoreGive(mutexes[sensorNumber]);
        } else {
            Serial.print("[weight] Sensor not found: ");
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

        if (false) {
            long newOffset = scaleSensors[i].read_average(10);
            state.offsets[i] = newOffset;
        }

        scaleSensors[i].set_offset(state.offsets[i]);
        scaleSensors[i].set_scale(config.scale);

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

    memcpy(outputBuffers, &buffers, sizeof(buffers));

    for (int i = 0; i < SENSOR_AMOUNT; i++)
    {
        int bufferSize = buffers[i].position;
        Serial.print("[weight] buffer size: ");
        Serial.println(bufferSize);
        buffers[i].position = 0;
    }

    for (int i = 0; i < SENSOR_AMOUNT; i++)
    {
        xSemaphoreGive(mutexes[i]);
    }
}
