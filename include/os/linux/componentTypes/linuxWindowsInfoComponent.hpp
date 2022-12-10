#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "log.hpp"

#include <array>


struct windowInfo
{
    windowId id;
    surfaceId topLevelSurface;
    std::array<surfaceId, 12> subsurfaces;
};

class windowsInfoComponent
{
    private:
        entityPool *pool;

        std::vector<windowInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            windowsInfoComponent *This = static_cast<windowsInfoComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        windowsInfoComponent(entityPool *pool);
        ~windowsInfoComponent();

        void deleteComponent(entityId id);
        windowInfo *getComponent(entityId id);
        void setComponent(entityId id, windowInfo buffer);
        std::vector<windowInfo>& getData()
        {
            return data;
        }
};
