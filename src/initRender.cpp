#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "initState.h"

#define i2cDisplayAddress 0x27
#define buttonPin 27

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
  TestScreen,
};

ScreenState SCREENS[] = {IpScreen, TestScreen};
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
      case TestScreen: {
        display.setCursor(0, 0);
        display.print("TEST");
        break;
      }
    }
  }
}

void switchScreen() {
  Serial.println("SWITCH!");
}

void renderBegin() {
  display.init();
  display.clear();
  display.setCursor(0, 0);
  display.print("Starting...");
  display.backlight();

  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleInterrupt, FALLING);
}
