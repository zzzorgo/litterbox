#ifndef LiterboxReport
#define LiterboxReport

#include <Arduino.h>
#include "initTime.h"

bool reporterBegin();
bool removeAllData();
bool reportWeight(UnixTime dateString, int catWeight, float pooWeight);
bool printData(Print* ptintTarget);
bool removeAllData();
bool persistState();

#endif
