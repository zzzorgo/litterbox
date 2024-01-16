#ifndef LiterboxTime
#define LiterboxTime

#include <Arduino.h>

using TimeMs = time_t;

void timeBegin();
String getFormatedTime();
TimeMs getUnixTime();

#endif
