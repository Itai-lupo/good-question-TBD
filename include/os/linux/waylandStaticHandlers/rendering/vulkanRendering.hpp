#pragma once

#include "core.hpp"
#include "surface.hpp"
#include "gpuRenderData.hpp"
#include "gpuRenderInfoComponent.hpp"
#include "renderApi.hpp"

#include <thread>
#include <wayland-client.h>

class vulkanRendering
{
    private:
        /* data */
        static inline gpuRenderInfoComponent *renderData;
        static inline renderApi *api;
    public:

        static void init(entityPool *surfacesPool, renderApi *api);
        static void close();

        static void allocateSurfaceToRender(surfaceId winId, void(*callback)(const gpuRenderData&));
        static void setRenderEventListeners(surfaceId winId, void(*callback)(const gpuRenderData&));
        
        static void deallocateSurfaceToRender(surfaceId winId);        
        
        static void resize(surfaceId id, int width, int height);

};