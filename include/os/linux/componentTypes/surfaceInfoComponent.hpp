#pragma once 
#include <wayland-client.h>
#include "core.hpp"
#include "entityPool.hpp"
#include "log.hpp"
#include "osAPI.hpp"

#include <array>


enum class surfaceRule
{
    layer,
    topLevel,
    popup,
    subsurface
};


struct surfaceData
{
    surfaceRule rule;
    surfaceRenderAPI rendererType;

    wl_surface *surface;
    int width, height;
    surfaceId id;
    windowId parentWindowId;
};

class surfaceInfoComponent
{
    private:
        entityPool *pool;

        std::vector<surfaceData> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            surfaceInfoComponent *This = static_cast<surfaceInfoComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        surfaceInfoComponent(entityPool *pool);
        ~surfaceInfoComponent();

        void deleteComponent(entityId id);
        surfaceData *getComponent(entityId id);
        void setComponent(entityId id, surfaceData buffer);
        std::vector<surfaceData>& getData()
        {
            return data;
        }
};
