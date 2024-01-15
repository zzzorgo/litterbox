#include <Arduino.h>

const char *ntpServer = "time.google.com";

void timeBegin() {
  configTime(0, 3600, ntpServer);
}

String getFormatedTime() {
  struct tm timeStruct;
  String str;
  str.reserve(20);

  if(!getLocalTime(&timeStruct)) {
    Serial.println("[time] Could not obtain time info");
    str += F("error");

    return str;
  }

  str += (timeStruct.tm_year + 1900);
  str += F("-");

  char* monthString = (char*) malloc(4);
  sprintf(monthString, "%02d-", timeStruct.tm_mon + 1);
  str += F(monthString);

  char* dayString = (char*) malloc(4);
  sprintf(dayString, "%02dT", timeStruct.tm_mday);
  str += F(dayString);

  char* hourString = (char*) malloc(4);
  sprintf(hourString, "%02d:", timeStruct.tm_hour);
  str += F(hourString);

  char* minString = (char*) malloc(4);
  sprintf(minString, "%02d:", timeStruct.tm_min);
  str += F(minString);

  char* secString = (char*) malloc(3);
  sprintf(secString, "%02d", timeStruct.tm_sec);
  str += F(secString);

  free(monthString);
  free(dayString);
  free(hourString);
  free(minString);
  free(secString);

  return str;
}
