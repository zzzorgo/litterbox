#ifndef LiterboxStateHeader
#define LiterboxStateHeader

#include <WiFi.h>

#define UNDEFINED_VALUE -1

enum LitterBoxState {
  InitialWeight,
  StableWeight,
  SoftStableWeight,
  UnstableWeight,
};

struct State {
    LitterBoxState litterBoxState;
    IPAddress ip;
    int pooCount;
    int catWeight;
    long offsets[4];
};

extern State state;

#endif
