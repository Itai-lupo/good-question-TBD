#include <vulkan/vulkan.hpp>

#include "commandBuffers.hpp"
#include "device.hpp"
#include "queueFamilys.hpp"
#include "log.hpp"

namespace vulkanRenderEngine
{
    void commandBuffers::init(entityPool *commandBuffersPool)
    {
        commandPoolsPool = new entityPool(100);
        commandBuffers::commandBuffersPool = commandBuffersPool;// commandBuffersPool;
        
        commandBuffersData = new commandBuffersComponents(commandBuffersPool, commandPoolsPool);
    }


    void commandBuffers::close()
    {
        for(int i = 0; i < commandBuffersData->getData().size(); i++){
            delete commandBuffersData->getData()[i].buffers;
            destroyCommandPool(commandBuffersData->getData()[i]);
        }

        delete commandBuffersData;

    }


    commandPoolId commandBuffers::createPool(commandPoolInfo& data)
    {
        commandPoolId id = commandPoolsPool->allocEntity();
        data.id = id;
        createCommandPool(data);
        commandBuffersData->setPool(data.id, data);
        return data.id;
    }


    void commandBuffers::setPool(commandPoolInfo& data)
    {
        destroyPool(data.id);
        createCommandPool(data);
        commandBuffersData->setPool(data.id, data);
    }


    commandPoolInfo *commandBuffers::getPool(commandBufferId id)
    {
        return commandBuffersData->getPool(id);

    }

    void commandBuffers::destroyPool(commandBufferId id)
    {
        destroyCommandPool(*commandBuffersData->getPool(id));

        commandBuffersData->deletePool(id);
    }




    commandBufferId commandBuffers::createBuffer(commandBufferInfo& data)
    {
        commandBufferId id = commandBuffersPool->allocEntity();
        data.id = id;
        createCommandBuffer(data);
        commandBuffersData->setBuffer(id, data);
        return id;
    }

    void commandBuffers::setBuffer(commandBufferInfo& data)
    {
        createCommandBuffer(data);
        commandBuffersData->setBuffer(data.id, data);
    }


    commandBufferInfo *commandBuffers::getBuffer(commandBufferId id)
    {
        return commandBuffersData->getBuffer(id);
    }

    void commandBuffers::destroyBuffer(commandBufferId id)
    {
        destroyCommandBuffer(commandBuffersData->getBuffer(id));

        commandBuffersData->deleteBuffer(id);
    }



    void commandBuffers::createCommandPool(commandPoolInfo& info)
    {
        vk::CommandPoolCreateInfo poolInfo;
        poolInfo.flags = info.flags;
        poolInfo.queueFamilyIndex = info.queueFamilyIndex;

        try {
			info.vkObject = device::getDevice().createCommandPool(poolInfo);
		}
		catch (vk::SystemError err) {
            LOG_FATAL("failed to create command pool! " << err.what());
		}
    }


    void commandBuffers::destroyCommandPool(commandPoolInfo& info)
    {
		device::getDevice().destroyCommandPool(info.vkObject);
    }


    void commandBuffers::createCommandBuffer(commandBufferInfo& info)
    {
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.commandPool =  commandBuffersData->getPool(info.poolId)->vkObject;
        allocInfo.level = info.level;
        allocInfo.commandBufferCount = 1;

        try {
			info.vkObject = device::getDevice().allocateCommandBuffers(allocInfo)[0];
		}
		catch (vk::SystemError err) {
            LOG_FATAL("failed to allocate command buffers! " << err.what());
		}

    }
    
    void commandBuffers::destroyCommandBuffer(commandBufferInfo *info)
    {
        device::getDevice().freeCommandBuffers(commandBuffersData->getPool(info->poolId)->vkObject, info->vkObject);
    }
}