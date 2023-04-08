#pragma once 

#include "core.hpp"
#include "entityPool.hpp"
#include <vulkan/vulkan.hpp>

#include <array>


class vkSurfaceComponents 
{
    private:
        entityPool *pool;

        std::vector<vk::SurfaceKHR> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            vkSurfaceComponents  *This = static_cast<vkSurfaceComponents *>(data);
            This->deleteComponent(id);
        }

    public:
        vkSurfaceComponents (entityPool *pool);
        ~vkSurfaceComponents ();

        void deleteComponent(entityId id);
        vk::SurfaceKHR getComponent(entityId id);
        void setComponent(entityId id, vk::SurfaceKHR buffer);
        std::vector<vk::SurfaceKHR>& getData()
        {
            return data;
        }
};
