#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "windowResizeData.hpp"
#include "log.hpp"


#include <wlr-layer-shell-client-protocol.h>
#include <wayland-client.h>

#include <vector>

struct layerSurfaceInfo
{
    /* data */
    surfaceId id;
    std::string title;
    bool canRender;

    zwlr_layer_surface_v1 *layerSurface;
};


class layerInfoComponenet
{
    private:
        entityPool *pool;

        std::vector<layerSurfaceInfo> data;
        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            layerInfoComponenet *This = static_cast<layerInfoComponenet *>(data);
            This->deleteComponent(id);
        }

    public:
        layerInfoComponenet(entityPool *pool);
        ~layerInfoComponenet();

        void deleteComponent(entityId id);
        layerSurfaceInfo *getComponent(entityId id);
        void setComponent(entityId id, layerSurfaceInfo buffer);
        std::vector<layerSurfaceInfo>& getData()
        {
            return data;
        }
};
