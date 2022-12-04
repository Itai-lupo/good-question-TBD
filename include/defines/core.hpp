#pragma once
#include <cstdint>


struct entityId
{
    uint32_t index;
    uint8_t gen;
};

struct windowId
{
    uint8_t gen;
    uint8_t index;
};


struct framebufferId
{
    uint32_t gen: 8 = (uint32_t)-1;
    uint32_t index: 24 = (uint32_t)-1;
};

struct textureId
{
    uint32_t gen: 8 = (uint32_t)-1; 
    uint32_t index: 24 = (uint32_t)-1;
};

struct vaoId
{
    uint32_t gen: 8 = (uint32_t)-1;
    uint32_t index: 24 = (uint32_t)-1;
};


struct shaderId
{
    uint32_t gen: 8 = (uint32_t)-1;
    uint32_t index: 24 = (uint32_t)-1;
};