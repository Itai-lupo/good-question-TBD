#include <vulkan/vulkan.hpp>

#include "commandBuffers.hpp"
#include "device.hpp"
#include "queueFamilys.hpp"
#include "log.hpp"

namespace vulkanRenderEngine
{
    void commandBuffers::init(entityPool *commandBuffersPool)
    {
        commandBuffers::commandBuffersPool = commandBuffersPool;// commandBuffersPool;
        commandBuffersData = new commandBuffersComponents(commandBuffersPool);


        vk::CommandPoolCreateInfo poolInfo;
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        poolInfo.queueFamilyIndex = queueFamilys::getGraphicsFamilyQueue().value();

        try {
			commandPool = device::getDevice().createCommandPool(poolInfo);
		}
		catch (vk::SystemError err) {
            LOG_FATAL("failed to create command pool! " << err.what());
		}


    }

    void commandBuffers::close()
    {
        for(int i = 0; i < commandBuffersData->getData().size(); i++)
            destroycommandBuffer(&commandBuffersData->getData()[i]);

        delete commandBuffersData;

		device::getDevice().destroyCommandPool(commandPool);

    }



    commandBufferId commandBuffers::create(commandBufferInfo& data)
    {
        commandBufferId id = commandBuffersPool->allocEntity();
        data.id = id;
        createcommandBuffer(data);
        commandBuffersData->setComponent(id, data);
        return id;
    }

    void commandBuffers::set(commandBufferInfo& data)
    {
        createcommandBuffer(data);
        commandBuffersData->setComponent(data.id, data);
    }


    commandBufferInfo *commandBuffers::get(commandBufferId id)
    {
        return commandBuffersData->getComponent(id);

    }

    void commandBuffers::destroy(commandBufferId id)
    {
        destroycommandBuffer(commandBuffersData->getComponent(id));

        commandBuffersData->deleteComponent(id);
    }





    void commandBuffers::createcommandBuffer(commandBufferInfo& info)
    {
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.commandPool = commandPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;

        try {
			info.vkObject = device::getDevice().allocateCommandBuffers(allocInfo)[0];
		}
		catch (vk::SystemError err) {
            LOG_FATAL("failed to allocate command buffers! " << err.what());
		}

    }
    
    void commandBuffers::destroycommandBuffer(commandBufferInfo *info)
    {

    }
}