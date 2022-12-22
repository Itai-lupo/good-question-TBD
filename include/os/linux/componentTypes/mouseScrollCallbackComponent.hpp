#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "mouseScrollData.hpp"
#include "log.hpp"


#include <array>

using scrollCallback = void(*)(const mouseScrollData&);

class mouseScrollCallbackComponent
{
    private:
        entityPool *pool;

        std::vector<scrollCallback> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            mouseScrollCallbackComponent *This = static_cast<mouseScrollCallbackComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        mouseScrollCallbackComponent(entityPool *pool);
        ~mouseScrollCallbackComponent();

        void deleteComponent(entityId id);
        scrollCallback getCallback(entityId id);
        void setCallback(entityId id, scrollCallback callback);
        std::vector<scrollCallback>& getData()
        {
            return data;
        }
};
