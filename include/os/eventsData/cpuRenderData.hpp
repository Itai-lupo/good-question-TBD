#pragma once
#include "core.hpp"

struct cpuRenderData
{
    surfaceId id;
    int width;
    int height;
    uint32_t *data;
    int deltaTime;

    cpuRenderData(surfaceId id, int width, int height, uint32_t *data, int deltaTime): 
        id(id), width(width), height(height), data(data), deltaTime(deltaTime) {}
        
};
