#include <Arduino.h>
#include <HX711.h>

const int LOADCELL_DOUT_PIN = 14;
const int LOADCELL_SCK_PIN = 12;

HX711 scale;
bool setTare = false;

void setup()
{
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void loop()
{
  if (scale.wait_ready_timeout(1000)) {
    long reading = scale.get_units();
    Serial.print("HX711 reading: ");
    Serial.println(reading);
  } else {
    Serial.println("HX711 not found.");
  }

  if (!setTare) {
    scale.tare(10);
    setTare = true;
    scale.set_scale(417.7);
  }
  // 47200 113
  // 0 0

  delay(1500);
}

