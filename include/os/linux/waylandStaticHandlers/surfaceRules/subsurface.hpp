#pragma once

#include <xdg-decoration-client-protocol.h>
#include <xdg-shell-client-protocol.h>
#include <wayland-client.h>

#include <string>
#include <vector>
#include <functional>

#include "core.hpp"
#include "surface.hpp"

class subsurface
{
    private:
    
    public:
        static inline wl_subcompositor *subcompositor;

        struct idToSurfaceDataIndexes
        {
            uint8_t gen = -1;
            uint8_t index = -1;
        };
        
        static inline std::vector<idToSurfaceDataIndexes> idToIndex;  

        struct subsurfaceInfo
        {
            /* data */
            surfaceId id;
    
            int x, y;
            wl_subsurface *subsurface;
        };

        static inline std::vector<subsurfaceInfo> topLevelSurfaces;
        

        static void allocateSubsurface(surfaceId winId, wl_surface *s, const surfaceSpec& surfaceData);
        static void deallocateTopLevel(surfaceId winId);      
        
};