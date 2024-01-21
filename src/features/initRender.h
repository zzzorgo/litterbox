#ifndef LiterboxRenderHeader
#define LiterboxRenderHeader

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>

#include "initState.h"
#include "../pinConfig.h"

void renderState();
void renderBegin(GpioNums buttonPin);
void renderSleep();

#endif
