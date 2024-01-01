#ifndef LiterboxReport
#define LiterboxReport

#include <Arduino.h>

bool reporterBegin();
bool removeAllData();
bool reportWeight(const String& dateString, float catWeight, float pooWeight);
bool printData(Print* ptintTarget);
bool removeAllData();
bool persistState();

#endif
