#pragma once
#include <cstdint>


struct entityId
{
    uint32_t index = (uint32_t)-1;
    uint8_t gen = (uint8_t)-1;
};

using windowId = entityId;
using framebufferId = entityId;
using textureId = entityId;
using vaoId = entityId;
using shaderId = entityId;
using uniformBufferId = entityId;
using surfaceId = entityId;
