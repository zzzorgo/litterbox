#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "initState.h"
#include "../pinConfig.h"

#define i2cDisplayAddress 0x27

LiquidCrystal_I2C display(i2cDisplayAddress, 16, 2);
volatile bool buttonInterruptionOccured = true;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR handleInterrupt() {
  portENTER_CRITICAL_ISR(&mux);
  buttonInterruptionOccured = true;
  portEXIT_CRITICAL_ISR(&mux);
}

enum ScreenState {
  IpScreen,
  PooCount,
};

ScreenState SCREENS[] = {IpScreen, PooCount};
int screenCount = sizeof(SCREENS) / sizeof(SCREENS[0]);
int screenIndex = screenCount - 1;

void renderState() {
  bool rerender = false;

  if (buttonInterruptionOccured) {
    rerender = true;

    if (screenIndex == screenCount - 1) {
      screenIndex = 0;
    } else {
      screenIndex++;
    }

    portENTER_CRITICAL(&mux);
    buttonInterruptionOccured = false;
    portEXIT_CRITICAL(&mux);
  }

  if (rerender) {
    ScreenState currentScreen = SCREENS[screenIndex];
    display.clear();

    switch (currentScreen) {
      case IpScreen: {
        display.setCursor(0, 0);
        display.print("IP");

        display.setCursor(0, 1);
        display.print(state.ip);
        break;
      }
      case PooCount: {
        display.setCursor(0, 0);

        char* pooCountString = (char*) malloc(5);
        sprintf(pooCountString, "% 4d",state.pooCount);

        display.print("Poo count:  ");
        display.print(pooCountString);

        display.setCursor(0, 1);
        display.print("Cat weight: ");

        if (state.catWeight == UNDEFINED_VALUE) {
          display.print(" N/A");
        } else {
          display.print(state.catWeight);
        }

        free(pooCountString);
        break;
      }
    }
  }
}

void switchScreen() {
  Serial.println("SWITCH!");
}

void renderBegin(GpioNums buttonPin) {
  display.init();
  display.clear();
  display.setCursor(0, 0);
  display.print("Starting...");
  display.backlight();

  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleInterrupt, FALLING);
}

void renderSleep() {
  display.noBacklight();
  display.noDisplay();
}
