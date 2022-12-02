#pragma once
#include "openGLRenderer.hpp"

struct windowRenderData
{
    int width;
    int height;
    uint32_t *data;
    int deltaTime;
    openGLRenderer* api;
    framebufferId buffer;

    windowRenderData(int width, int height, uint32_t *data, int deltaTime): 
        width(width), height(height), data(data), deltaTime(deltaTime) {}
    windowRenderData(int width, int height, openGLRenderer* api, int deltaTime, framebufferId buffer): 
        width(width), height(height), api(api), deltaTime(deltaTime), buffer(buffer) {}
};
