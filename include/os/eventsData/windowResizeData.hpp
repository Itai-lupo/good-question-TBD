#pragma once

#include "windowState.hpp"

struct windowResizeData
{
    surfaceId id;
    int32_t width; 
    int32_t height; 
    windowSizeState state;
};
