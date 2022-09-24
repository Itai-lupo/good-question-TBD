#pragma once

#include "core.hpp"
#include "windowResizeData.hpp"
#include "osAPI.hpp"


#include <vector>
#include <list>
#include <string>
#include <functional>
#include <wayland-client.h>

struct surfaceId
{
    uint16_t gen;
    uint16_t index;
};

enum class surfaceRule
{
    topLevel,
    popup,
    subsurface
};

struct surfaceSpec
{
    surfaceRule rule;
	int width, height;
	int x, y;
	
    std::string title;
    surfaceId parentSurface;

};


class surface
{
    private:


    public:
        static inline wl_compositor *compositor = NULL;
        static inline wl_registry *registry;
        struct idToSurfaceDataIndexes
        {
            uint8_t gen;
            uint8_t surfaceDataIndex = -1;
        };
        
        static inline std::vector<idToSurfaceDataIndexes> idToIndex;  
        static inline std::list<uint32_t> freeSlots;

        struct surfaceData
        {
            surfaceRule rule;
            wl_surface *surface;
            int width, height;
            surfaceId id;
            windowId parentWindowId;

        };

        static inline std::vector<surfaceData> surfaces;
        
        static surfaceId allocateSurface(windowId winId, const surfaceSpec& surfaceData);

        static void deallocateSurface(surfaceId winId);       

        static void setWindowHeight(surfaceId id, int height);
        static int getWindowHeight(surfaceId id);

        static void setWindowWidth(surfaceId id, int width);
        static int getWindowWidth(surfaceId id);

        static wl_surface *getSurface(surfaceId id)
        {
            if(idToIndex[id.index].surfaceDataIndex != (uint8_t)-1 && id.gen == idToIndex[id.index].gen)
                return surfaces[idToIndex[id.index].surfaceDataIndex].surface;
    
            return nullptr;
        }
};
