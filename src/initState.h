#ifndef LiterboxStateHeader
#define LiterboxStateHeader

#include <WiFi.h>

enum LitterBoxState {
  Undefined,
  Ready,
  CatInside,
  Maintenance
};

struct State {
    LitterBoxState litterBoxState;
    IPAddress ip;
};

extern State state;

#endif
