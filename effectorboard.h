#pragma once
#include "effectors.h"


/*
Example
SequentialBoard board = {
    Distortion{30, 0.3},
    Overdrive{3},
    Bitcrusher{4, 6}
};
*/

/*
SequentialBoard board = {
    Overdrive{5},
    Bitcrusher{4, 12}
};
*/

/*
SequentialBoard board = {
    Delay{0.8f, 0.4f, 0.15}
};
*/

SequentialBoard board = {
    Delay{0.4f, 0.5f, 0.3f},
    Tremolo{5.0f, 0.6f}
};