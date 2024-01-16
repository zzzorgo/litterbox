#ifndef LiterboxReport
#define LiterboxReport

#include <Arduino.h>
#include "initTime.h"

bool reporterBegin();
bool removeAllData();
bool reportWeight(TimeMs dateString, float catWeight, float pooWeight);
bool printData(Print* ptintTarget);
bool removeAllData();
bool persistState();

#endif
