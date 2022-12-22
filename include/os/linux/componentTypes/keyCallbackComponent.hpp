#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "keyData.hpp"
#include "log.hpp"


#include <array>

using keyCallback = void(*)(const keyData&);

class keyCallbackComponent
{
    private:
        entityPool *pool;

        std::vector<keyCallback> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            keyCallbackComponent *This = static_cast<keyCallbackComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        keyCallbackComponent(entityPool *pool);
        ~keyCallbackComponent();

        void deleteComponent(entityId id);
        keyCallback getCallback(entityId id);
        void setCallback(entityId id, keyCallback callback);
        std::vector<keyCallback>& getData()
        {
            return data;
        }
};
