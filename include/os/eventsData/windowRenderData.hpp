#pragma once
#include "openGLRenderer.hpp"

struct windowRenderData
{
    surfaceId id;
    int width;
    int height;
    uint32_t *data;
    int deltaTime;
    openGLRenderer* api;
    framebufferId buffer;

    windowRenderData(surfaceId id, int width, int height, uint32_t *data, int deltaTime): 
        id(id), width(width), height(height), data(data), deltaTime(deltaTime) {}
        
    windowRenderData(surfaceId id, int width, int height, openGLRenderer* api, int deltaTime, framebufferId buffer): 
        id(id), width(width), height(height), api(api), deltaTime(deltaTime), buffer(buffer) {}
};
