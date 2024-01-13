#include <Arduino.h>
#include <HX711.h>

const int LOADCELL_DOUT_PIN = 14;
const int LOADCELL_SCK_PIN = 12;

HX711 scale;
bool setTare = false;

#include <Arduino.h>

void taskOne(void *parameter)
{
  while (true)
  {
    if (scale.wait_ready_timeout(1000))
    {
      long reading = scale.get_units();
      float readingF = reading;
      float weight = readingF / 10;

      char *readingString = (char *)malloc(30);
      sprintf(readingString, "weight: %.1f g", weight);
      Serial.println(readingString);
      free(readingString);
    }
    else
    {
      Serial.println("HX711 not found.");
    }

    if (!setTare)
    {
      scale.tare(10);
      setTare = true;
      scale.set_scale(41.77);
    }
    taskYIELD();
  }
}

void taskTwo(void *parameter)
{
  for (int i = 0; i < 10; i++)
  {
    Serial.println("Hello from task 2");
    delay(1000);
  }
  Serial.println("Ending task 2");
  vTaskDelete(NULL);
}

void setup()
{
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  xTaskCreate(
      taskOne,   /* Task function. */
      "TaskOne", /* String with name of task. */
      10000,     /* Stack size in bytes. */
      NULL,      /* Parameter passed as input of the task */
      tskIDLE_PRIORITY,         /* Priority of the task. */
      NULL);     /* Task handle. */

  xTaskCreate(
      taskTwo,   /* Task function. */
      "TaskTwo", /* String with name of task. */
      10000,     /* Stack size in bytes. */
      NULL,      /* Parameter passed as input of the task */
      tskIDLE_PRIORITY,         /* Priority of the task. */
      NULL);
}

void loop()
{
  delay(1000);
}
