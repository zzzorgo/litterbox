#ifndef LiterboxTime
#define LiterboxTime

#include <Arduino.h>

using UnixTime = time_t;
using UnixTimeMs = int64_t;

void timeBegin();
String getFormatedTime();
UnixTime getUnixTime();
UnixTimeMs getUnixTimeMs();

#endif
