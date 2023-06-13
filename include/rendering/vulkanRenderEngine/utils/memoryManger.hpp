#pragma once
#include "core.hpp"
#include "entityPool.hpp"
#include "renderComponentTemplate.hpp"

#include <vulkan/vulkan.hpp>

using vkMemoryId = entityId;

namespace vulkanRenderEngine
{
    struct memoryBuffer
    {
        size_t bufferSize;
        vk::Flags<vk::BufferUsageFlagBits> usage;
        vk::SharingMode sharingMode;

        vkMemoryId id;
        vk::Buffer vkBuffer;
        VkDeviceMemory vertexBufferMemory;
    };

    class memoryManger
    {
    private:
        static inline entityPool *pool;
        static inline renderComponentTemplate *buffers;

        static uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

    public:
        static void init() noexcept;
        static void close() noexcept;

        static vkMemoryId mapBuffer(memoryBuffer info);
        static void writeToBuffer(vkMemoryId id, void *buffer);
        static void unmapBuffer(vkMemoryId id);
        static void resizeBuffer(vkMemoryId id);
        static memoryBuffer &getBuffer(vkMemoryId id);
    };
}