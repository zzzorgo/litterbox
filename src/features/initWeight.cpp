#include <HX711.h>

#include "initWeight.h"

ScaleSensor scaleSensors[SENSOR_AMOUNT];
bool needResetZero[SENSOR_AMOUNT] = {true, true, true, true};

void weightingTask(void *parameter)
{
    int *sensorNumberPointer = (int *)parameter;
    int sensorNumber = *sensorNumberPointer;
    ScaleSensor scaleSensor = scaleSensors[sensorNumber];

    while (true)
    {
        if (scaleSensor.wait_ready_timeout(1000))
        {
            // Serial.print("Sensor set: ");
            // Serial.println(sensorNumber);
            long reading = scaleSensor.get_units();
            float readingFloat = reading;
            float weight = readingFloat / 10;

            char *readingString = (char *)malloc(30);
            sprintf(readingString, "weight %d: %.1f g", sensorNumber, weight);
            Serial.println(readingString);
            free(readingString);
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
            2500,
            sensorNumber,
            tskIDLE_PRIORITY,
            NULL /* Task handle. */
        );
    }
}
