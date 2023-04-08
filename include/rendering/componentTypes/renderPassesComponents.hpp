#pragma once 

#include "core.hpp"
#include "entityPool.hpp"

#include <array>
#include <string>
#include <vulkan/vulkan.hpp>

struct renderPassInfo
{
    renderPassId id;
    
    vk::Format format;
    vk::RenderPass vkObject;
    
};


class renderPassesComponents 
{
    private:
        entityPool *pool;

        std::vector<renderPassInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            renderPassesComponents  *This = static_cast<renderPassesComponents *>(data);
            This->deleteComponent(id);
        }

    public:
        renderPassesComponents (entityPool *pool);
        ~renderPassesComponents ();

        void deleteComponent(entityId id);
        renderPassInfo *getComponent(entityId id);
        void setComponent(entityId id, renderPassInfo& buffer);
        std::vector<renderPassInfo>& getData()
        {
            return data;
        }
};
