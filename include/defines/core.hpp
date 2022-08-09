#pragma once
#include <cstdint>

template <std::size_t genSize, std::size_t idSize>
struct genericHandle
{
    uint32_t gen: genSize;
    uint32_t index: idSize;
};



using windowId = genericHandle<24, 8>;
