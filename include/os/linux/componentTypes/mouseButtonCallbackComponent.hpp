#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "mouseButtonData.hpp"
#include "log.hpp"


#include <array>

using mouseCallback = void(*)(const mouseButtonData&);

class mouseButtonCallbackComponent
{
    private:
        entityPool *pool;

        std::vector<mouseCallback> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            mouseButtonCallbackComponent *This = static_cast<mouseButtonCallbackComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        mouseButtonCallbackComponent(entityPool *pool);
        ~mouseButtonCallbackComponent();

        void deleteComponent(entityId id);
        mouseCallback getCallback(entityId id);
        void setCallback(entityId id, mouseCallback callback);
        std::vector<mouseCallback>& getData()
        {
            return data;
        }
};
