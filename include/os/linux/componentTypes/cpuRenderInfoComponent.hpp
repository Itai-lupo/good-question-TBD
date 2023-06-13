#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "log.hpp"
#include "cpuRenderData.hpp"

#include <array>



#include <thread>
#include <memory>
#include <shared_mutex>
#include <condition_variable>

#include <wayland-client.h>



struct cpuRenderInfo
{
    surfaceId id;

    //to do move into a cpu rendering system
    int fd;
    wl_shm_pool *pool;
    uint32_t memoryPoolSize;
    uint8_t bufferInRender;
    uint8_t bufferToRender;
    uint8_t freeBuffer;
    uint32_t *buffer;
    int bufferSize;

    std::thread *renderThread = nullptr;

    bool renderFinshedBool;

    void(*renderFuncion)(const cpuRenderData&);

    cpuRenderInfo(void(*callback)(const cpuRenderData&)): bufferInRender(0), bufferToRender(1), freeBuffer(2),
        renderFuncion(callback)
    {
        
    }
};

class cpuRenderInfoComponent
{
    private:
        entityPool *pool;

        std::vector<cpuRenderInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            cpuRenderInfoComponent *This = static_cast<cpuRenderInfoComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        cpuRenderInfoComponent(entityPool *pool);
        ~cpuRenderInfoComponent();

        void deleteComponent(entityId id);
        cpuRenderInfo *getComponent(entityId id);
        void setComponent(entityId id, cpuRenderInfo& buffer);
        std::vector<cpuRenderInfo>& getData()
        {
            return data;
        }
};
