#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "windowResizeData.hpp"
#include "log.hpp"


#include <array>

using surfaceCallback = void(*)(surfaceId winId);

class windowSurfaceCallbackComponent
{
    private:
        entityPool *pool;

        std::vector<surfaceCallback> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            windowSurfaceCallbackComponent *This = static_cast<windowSurfaceCallbackComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        windowSurfaceCallbackComponent(entityPool *pool);
        ~windowSurfaceCallbackComponent();

        void deleteComponent(entityId id);
        surfaceCallback getCallback(entityId id);
        void setCallback(entityId id, surfaceCallback callback);
        std::vector<surfaceCallback>& getData()
        {
            return data;
        }
};