#include "initState.h"

State state = {
    .litterBoxState = InitialWeight,
    .ip = WiFi.localIP(),
    .pooCount = 0,
    .catWeight = 0,
};
