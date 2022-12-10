#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "windowResizeData.hpp"
#include "log.hpp"


#include <array>

using resizeCallback = void(*)(const windowResizeData&);

class windowResizeCallbackComponent
{
    private:
        entityPool *pool;

        std::vector<resizeCallback> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            windowResizeCallbackComponent *This = static_cast<windowResizeCallbackComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        windowResizeCallbackComponent(entityPool *pool);
        ~windowResizeCallbackComponent();

        void deleteComponent(entityId id);
        resizeCallback getCallback(entityId id);
        void setCallback(entityId id, resizeCallback callback);
        std::vector<resizeCallback>& getData()
        {
            return data;
        }
};
