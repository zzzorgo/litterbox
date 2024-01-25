#ifndef LiterboxStateHeader
#define LiterboxStateHeader

#include <WiFi.h>

#define UNDEFINED_VALUE -1

enum LitterBoxState {
  Ready,
  Rising,
  Falling,
  StableHigh,
};

struct State {
    LitterBoxState litterBoxState;
    IPAddress ip;
    int pooCount;
    float catWeight;
    long offsets[4];
};

extern State state;

#endif
