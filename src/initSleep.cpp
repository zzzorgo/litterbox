#include <Arduino.h>

#include "initRender.h"
#include "initWifi.h"

void sleepBegin() {
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, LOW);
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
