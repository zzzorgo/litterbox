#include "initState.h"

State state = {
    .litterBoxState = Undefined,
    .ip = WiFi.localIP(),
    .pooCount = 0,
    .catWeight = -1.0,
};
