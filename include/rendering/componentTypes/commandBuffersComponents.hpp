#pragma once 

#include "core.hpp"
#include "entityPool.hpp"

#include <array>
#include <string>
#include <vulkan/vulkan.hpp>

using commandPoolId = entityId;

struct commandBufferInfo
{
    commandBufferId id;
    commandPoolId poolId;
    
    vk::CommandBuffer vkObject;

    vk::CommandBufferLevel level;
    
};

struct commandPoolInfo
{
    commandPoolId id;
    
    vk::CommandPool vkObject;

    vk::CommandPoolCreateFlags flags;
    uint32_t queueFamilyIndex;

    std::vector<commandBufferInfo> *buffers;
    
};


class commandBuffersComponents 
{
    private:
        entityPool *buffersPool;
        entityPool *poolsPool;

        std::vector<commandPoolInfo> data;
        
        uint32_t *poolIdToIndex;  

        union
        {
            uint32_t *idPoolBuffer;  
            struct
            {
                uint16_t poolId;
                uint16_t bufferIndex;
            } *poolAndIndex;
        } bufferIdToBuffer;

        

        static void poolDeleteCallback(void * data, entityId id)
        {
            commandBuffersComponents  *This = static_cast<commandBuffersComponents *>(data);
            This->deletePool(id);
        }


        static void bufferDeleteCallback(void * data, entityId id)
        {
            commandBuffersComponents  *This = static_cast<commandBuffersComponents *>(data);
            This->deleteBuffer(id);
        }

    public:
        commandBuffersComponents(entityPool *buffersPool, entityPool *poolsPool);
        ~commandBuffersComponents ();

        void deletePool(entityId id);
        commandPoolInfo *getPool(entityId id);
        void setPool(entityId id, commandPoolInfo& buffer);


        void deleteBuffer(entityId id);
        commandBufferInfo *getBuffer(entityId id);
        void setBuffer(entityId id, commandBufferInfo& buffer);


        std::vector<commandPoolInfo>& getData()
        {
            return data;
        }
};
