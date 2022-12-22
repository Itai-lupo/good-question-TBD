#pragma once

#include <xdg-decoration-client-protocol.h>
#include <xdg-shell-client-protocol.h>
#include <wayland-client.h>

#include <string>
#include <vector>
#include <functional>

#include "core.hpp"
#include "surface.hpp"
#include "subsurfaceInfoComponenet.hpp"

class subsurface
{
    private:
        static inline subsurfaceInfoComponenet *subsurfaces;

    public:
        static inline wl_subcompositor *subcompositor;

        static void init(entityPool *surfacesPool);
        static void close();

        static void allocateSubsurface(surfaceId winId, wl_surface *s, const surfaceSpec& surfaceData);
        static void deallocateTopLevel(surfaceId winId);      
        
};