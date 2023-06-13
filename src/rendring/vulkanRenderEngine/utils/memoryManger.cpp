#include "memoryManger.hpp"
#include "device.hpp"
#include "log.hpp"

namespace vulkanRenderEngine
{

    void memoryManger::init() noexcept
    {
        pool = new entityPool(1000);
        buffers = new renderComponentTemplate(pool, sizeof(memoryBuffer));
    }

    void memoryManger::close() noexcept
    {
        for (memoryBuffer *i = (memoryBuffer *)buffers->getData(); i < buffers->getLastElement(); i++)
        {
            device::getDevice().destroyBuffer(i->vkBuffer, nullptr);
            device::getDevice().freeMemory(i->vertexBufferMemory, nullptr);
        }

        delete buffers;
        delete pool;
    }

    vkMemoryId memoryManger::mapBuffer(memoryBuffer info)
    {
        vkMemoryId id = pool->allocEntity();
        info.id = id;
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.size = info.bufferSize;
        bufferInfo.usage = info.usage;
        bufferInfo.sharingMode = info.sharingMode;

        info.vkBuffer = device::getDevice().createBuffer(bufferInfo);
        vk::MemoryRequirements memRequirements = device::getDevice().getBufferMemoryRequirements(info.vkBuffer);

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        info.vertexBufferMemory = device::getDevice().allocateMemory(allocInfo);

        device::getDevice().bindBufferMemory(info.vkBuffer, info.vertexBufferMemory, 0);

        buffers->setComponent(id, (void *)&info);
        return id;
    }

    void memoryManger::writeToBuffer(vkMemoryId id, void *buffer)
    {
        memoryBuffer &info = buffers->getComponent<memoryBuffer>(id);

        void *data = device::getDevice().mapMemory(info.vertexBufferMemory, 0, info.bufferSize);
        memcpy(data, buffer, info.bufferSize);
        device::getDevice().unmapMemory(info.vertexBufferMemory);
    }

    void memoryManger::unmapBuffer(vkMemoryId id)
    {
        if (!buffers->isIdValid(id))
            return;

        memoryBuffer &info = buffers->getComponent<memoryBuffer>(id);

        device::getDevice().destroyBuffer(info.vkBuffer, nullptr);
        device::getDevice().freeMemory(info.vertexBufferMemory, nullptr);

        pool->freeEntity(id);
    }

    void memoryManger::resizeBuffer(vkMemoryId id)
    {
    }

    uint32_t memoryManger::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memProperties = device::getPhysicalDevice().getMemoryProperties();
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    memoryBuffer &memoryManger::getBuffer(vkMemoryId id)
    {
        return buffers->getComponent<memoryBuffer>(id);
    }

}
