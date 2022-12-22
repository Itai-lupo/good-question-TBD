#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "mouseMoveData.hpp"
#include "log.hpp"


#include <array>

using mouseMoveCallback = void(*)(const mouseMoveData&);

class mouseMoveCallbackComponent
{
    private:
        entityPool *pool;

        std::vector<mouseMoveCallback> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            mouseMoveCallbackComponent *This = static_cast<mouseMoveCallbackComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        mouseMoveCallbackComponent(entityPool *pool);
        ~mouseMoveCallbackComponent();

        void deleteComponent(entityId id);
        mouseMoveCallback getCallback(entityId id);
        void setCallback(entityId id, mouseMoveCallback callback);
        std::vector<mouseMoveCallback>& getData()
        {
            return data;
        }
};
