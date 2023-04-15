#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "log.hpp"
#include "gpuRenderData.hpp"

#include <array>



#include <thread>
#include <memory>
#include <shared_mutex>
#include <condition_variable>

#include <wayland-client.h>



struct vulkanRenderInfo
{
    surfaceId id;
    vkSurfaceId vkSurface;
    void(*renderFuncion)(const gpuRenderData&);
    
    vulkanRenderInfo(void(*callback)(const gpuRenderData&)):
        renderFuncion(callback)
    {
        
    }
};

class vulkanRenderInfoComponent
{
    private:
        entityPool *pool;

        std::vector<vulkanRenderInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            vulkanRenderInfoComponent *This = static_cast<vulkanRenderInfoComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        vulkanRenderInfoComponent(entityPool *pool);
        ~vulkanRenderInfoComponent();

        void deleteComponent(entityId id);
        vulkanRenderInfo *getComponent(entityId id);
        void setComponent(entityId id, vulkanRenderInfo buffer);
        std::vector<vulkanRenderInfo>& getData()
        {
            return data;
        }
};
