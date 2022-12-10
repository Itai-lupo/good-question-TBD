#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "windowResizeData.hpp"
#include "log.hpp"


#include <array>

using closeCallback = void(*)(windowId winId);

class windowCloseCallbackComponent
{
    private:
        entityPool *pool;

        std::vector<closeCallback> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            windowCloseCallbackComponent *This = static_cast<windowCloseCallbackComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        windowCloseCallbackComponent(entityPool *pool);
        ~windowCloseCallbackComponent();

        void deleteComponent(entityId id);
        closeCallback getCallback(entityId id);
        void setCallback(entityId id, closeCallback callback);
        std::vector<closeCallback>& getData()
        {
            return data;
        }
};