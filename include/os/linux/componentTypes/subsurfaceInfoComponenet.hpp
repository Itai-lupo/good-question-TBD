#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "windowResizeData.hpp"
#include "log.hpp"

#include <wayland-client.h>

#include <vector>



struct subsurfaceInfo
{
    /* data */
    surfaceId id;

    int x, y;
    wl_subsurface *subsurface;
    std::string title;
};

class subsurfaceInfoComponenet
{
    private:
        entityPool *pool;

        std::vector<subsurfaceInfo> data;
        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            subsurfaceInfoComponenet *This = static_cast<subsurfaceInfoComponenet *>(data);
            This->deleteComponent(id);
        }

    public:
        subsurfaceInfoComponenet(entityPool *pool);
        ~subsurfaceInfoComponenet();

        void deleteComponent(entityId id);
        subsurfaceInfo *getComponent(entityId id);
        void setComponent(entityId id, subsurfaceInfo buffer);
        std::vector<subsurfaceInfo>& getData()
        {
            return data;
        }
};