#include "initState.h"

State state = {
    .litterBoxState = Ready,
    .ip = WiFi.localIP(),
    .pooCount = 0,
    .catWeight = 0,
};
