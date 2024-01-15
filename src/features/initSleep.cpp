#include <Arduino.h>

#include "initRender.h"
#include "initWifi.h"
#include "../pinConfig.h"

void sleepBegin(GpioNums wakeUpPin) {
    esp_sleep_enable_ext0_wakeup(wakeUpPin, LOW);
}

void deepSleep() {
    wifiSleep();
    renderSleep();
    esp_deep_sleep_start();
}

void sleepOnTimeout() {
    if (millis() > 10000) {
        deepSleep();
    }
}
