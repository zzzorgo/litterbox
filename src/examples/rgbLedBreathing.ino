#include <Arduino.h>

#define redLedPin 17
#define blueLedPin 18
#define greenLedPin 16

enum Color {
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE,
};

struct Step {
  bool sign;
  Color color;
};

Step steps[] = {
  { .sign = true, .color = COLOR_GREEN },
  { .sign = false, .color = COLOR_RED },
  { .sign = true, .color = COLOR_BLUE },
  { .sign = false, .color = COLOR_GREEN },
  { .sign = true, .color = COLOR_RED },
  { .sign = false, .color = COLOR_BLUE },
};

int colors[3];
void setup()
{
  // Serial.begin(115200);
  colors[COLOR_RED] = 0;
  colors[COLOR_GREEN] = 255;
  colors[COLOR_BLUE] = 255;

  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);

  analogWrite(redLedPin, 255);
  analogWrite(blueLedPin, 255);
  analogWrite(greenLedPin, 255);
}

int currentStep = 0;

void render() {
  analogWrite(redLedPin, colors[COLOR_RED]);
  analogWrite(blueLedPin, colors[COLOR_BLUE]);
  analogWrite(greenLedPin, colors[COLOR_GREEN]);
}

void loop()
{
  Step step = steps[currentStep];

  for (int i = 0; i < 255; i++)
  {
    if (step.sign) {
      colors[step.color]--;
    } else {
      colors[step.color]++;
    }
    render();
    delay(5);
  }

  if (currentStep == 5) {
    currentStep = 0;
  } else {
    currentStep++;
  }
}
