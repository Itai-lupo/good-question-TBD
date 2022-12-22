#pragma once 

#include "core.hpp"
#include "entityPool.hpp"
#include "supportedRenderApis.hpp"

#include <array>


class apiTypeComponents 
{
    private:
        entityPool *pool;

        std::vector<supportedRenderApis> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            apiTypeComponents  *This = static_cast<apiTypeComponents *>(data);
            This->deleteComponent(id);
        }

    public:
        apiTypeComponents (entityPool *pool);
        ~apiTypeComponents ();

        void deleteComponent(entityId id);
        supportedRenderApis getComponent(entityId id);
        void setComponent(entityId id, supportedRenderApis buffer);
        std::vector<supportedRenderApis>& getData()
        {
            return data;
        }
};
