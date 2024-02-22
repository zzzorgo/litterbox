#include "initRender.h"

#define i2cDisplayAddress 0x27
#define LED_COUNT 4

LiquidCrystal_I2C display(i2cDisplayAddress, 16, 2);
Adafruit_NeoPixel strip(LED_COUNT, rgbPin, NEO_GRB + NEO_KHZ800);

enum ScreenState
{
  IpScreen,
  PooCount,
};

ScreenState SCREENS[] = {IpScreen, PooCount};
int screenCount = sizeof(SCREENS) / sizeof(SCREENS[0]);
int screenIndex = 0;
IPAddress nullIp = IPAddress();

bool buttonState = false;
bool prevButtonState = false;

void renderState()
{
  bool started = state.ip != nullIp;

  buttonState = digitalRead(buttonPin);

  if (prevButtonState == 1 && buttonState == 0)
  {
    state.rerender = true;

    if (screenIndex == screenCount - 1)
    {
      screenIndex = 0;
    }
    else
    {
      screenIndex++;
    }
  }

  prevButtonState = buttonState;

  if (state.rerender && started)
  {
    ScreenState currentScreen = SCREENS[screenIndex];
    display.clear();
    state.rerender = false;

    switch (currentScreen)
    {
    case IpScreen:
    {
      display.setCursor(0, 0);
      display.print("IP");

      display.setCursor(0, 1);
      display.print(state.ip);
      break;
    }
    case PooCount:
    {
      display.setCursor(0, 0);

      char *pooCountString = (char *)malloc(5);
      sprintf(pooCountString, "% 4d", state.pooCount);

      display.print("Poo count:  ");
      display.print(pooCountString);

      display.setCursor(0, 1);
      display.print("Cat weight: ");

      if (state.catWeight == 0)
      {
        display.print(" N/A");
      }
      else
      {
        display.print(state.catWeight);
      }

      free(pooCountString);
      break;
    }
    }
  }

  int pixelCount = strip.numPixels();

  for (int i = 0; i < pixelCount; i++)
  {
    strip.setPixelColor(i, 170, 255, 0);
  }

  int paintRedPixelCount = min(pixelCount, state.pooCount);

  for (int i = 0; i < paintRedPixelCount; i++)
  {
    strip.setPixelColor(i, 255, 20, 0);
  }

  strip.setBrightness(25);
  strip.show();
}

void renderLoop(void *param)
{
  while (true)
  {
    renderState();
    delay(100);
  }
}

void renderBegin(GpioNums buttonPin)
{
  display.init();
  display.clear();
  display.setCursor(0, 0);
  display.print("Starting...");
  display.backlight();

  strip.begin();

  pinMode(buttonPin, INPUT_PULLDOWN);

  xTaskCreate(renderLoop, "renderLoop", 2048, &buttonPin, tskIDLE_PRIORITY, NULL);
}

void renderSleep()
{
  display.noBacklight();
  display.noDisplay();
  strip.setBrightness(0);
}
