#pragma once

#include "core.hpp"
#include "windowResizeData.hpp"
#include "cpuRenderData.hpp"
#include "osAPI.hpp"

#include "entityPool.hpp"
#include "surfaceInfoComponent.hpp"


#include <vector>
#include <list>
#include <string>
#include <functional>
#include <wayland-client.h>

struct surfaceSpec
{
    surfaceRule rule;
    surfaceRenderAPI rendererType;
	int width, height;
	int x, y;
	
    std::string title;
    surfaceId parentSurface;

    void (*cpuRenderFunction)(const cpuRenderData &);
    void (*gpuRenderFunction)(const gpuRenderData &);

};


class surface
{
    private:
        static inline renderApi *api;
        
    public:
        static inline wl_compositor *compositor = NULL;
        static inline wl_registry *registry;

        static inline entityPool *surfacePool;
        static inline surfaceInfoComponent *surfacesInfo;

        
        static void init();

        static void close();
        
        static surfaceId allocateSurface(windowId winId, const surfaceSpec& surfaceData);
        static void deallocateSurface(surfaceId winId);       

        static void resize(surfaceId id, int width, int height);

        static void setWindowHeight(surfaceId id, int height);
        static int getWindowHeight(surfaceId id);

        static void setWindowWidth(surfaceId id, int width);
        static int getWindowWidth(surfaceId id);

        static wl_surface *getSurface(surfaceId id)
        {
            return surfacesInfo->getComponent(id)->surface;
        }
};
