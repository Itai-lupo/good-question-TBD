#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "windowResizeData.hpp"
#include "log.hpp"

#include <xdg-decoration-client-protocol.h>
#include <xdg-shell-client-protocol.h>
#include <wayland-client.h>

#include <vector>


struct toplevelSurfaceInfo
{
    /* data */
    surfaceId id;
    std::string title;

    int width, height;

    xdg_surface *xdgSurface; // to do add child windows support as well as popup support
    xdg_toplevel *xdgToplevel;
    zxdg_toplevel_decoration_v1 *topLevelDecoration;
};

class toplevelInfoComponenet
{
    private:
        entityPool *pool;

        std::vector<toplevelSurfaceInfo> data;
        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            toplevelInfoComponenet *This = static_cast<toplevelInfoComponenet *>(data);
            This->deleteComponent(id);
        }

    public:
        toplevelInfoComponenet(entityPool *pool);
        ~toplevelInfoComponenet();

        void deleteComponent(entityId id);
        toplevelSurfaceInfo *getComponent(entityId id);
        void setComponent(entityId id, toplevelSurfaceInfo buffer);
        std::vector<toplevelSurfaceInfo>& getData()
        {
            return data;
        }
};