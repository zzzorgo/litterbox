#ifndef LiterboxSleep
#define LiterboxSleep

#include "../pinConfig.h"

void sleepBegin(GpioNums wakeUpPin);
void sleepOnTimeout();

#endif
