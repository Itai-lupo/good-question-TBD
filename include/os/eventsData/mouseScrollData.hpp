#pragma once
#include "mouseFormat.hpp"

struct mouseScrollData
{
    surfaceId id;
    mouseAxis axis;
    mouseAxisSource axisSource;
    int value;
    int discreteValue;
};
