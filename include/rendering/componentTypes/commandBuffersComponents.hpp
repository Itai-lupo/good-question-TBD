#pragma once 

#include "core.hpp"
#include "entityPool.hpp"

#include <array>
#include <string>
#include <vulkan/vulkan.hpp>

struct commandBufferInfo
{
    commandBufferId id;
    
    vk::CommandBuffer vkObject;
    
};


class commandBuffersComponents 
{
    private:
        entityPool *pool;

        std::vector<commandBufferInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            commandBuffersComponents  *This = static_cast<commandBuffersComponents *>(data);
            This->deleteComponent(id);
        }

    public:
        commandBuffersComponents (entityPool *pool);
        ~commandBuffersComponents ();

        void deleteComponent(entityId id);
        commandBufferInfo *getComponent(entityId id);
        void setComponent(entityId id, commandBufferInfo& buffer);
        std::vector<commandBufferInfo>& getData()
        {
            return data;
        }
};
