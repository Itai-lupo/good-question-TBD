#pragma once
#include "core.hpp"
#include "supportedRenderApis.hpp"

#include <queue>
#include <stdlib.h>

class renderApi;
struct gpuRenderData
{
    surfaceId id;
    
    int width;
    int height;
    int deltaTime;

    framebufferId buffer;
    renderApi* api;
    supportedRenderApis apiType;
        
    gpuRenderData(surfaceId id, int width, int height, int deltaTime, renderApi* api, framebufferId buffer, supportedRenderApis apiType): 
        id(id), width(width), height(height), deltaTime(deltaTime), api(api), buffer(buffer), apiType(apiType) {}
};
