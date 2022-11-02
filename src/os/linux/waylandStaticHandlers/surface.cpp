#ifdef __linux__
#include "surface.hpp"
#include "cpuRendering.hpp"
#include "openGLRendering.hpp"
#include "linuxWindowAPI.hpp"
#include "log.hpp"
#include "toplevel.hpp"
#include "subsurface.hpp"
#include "keyboard.hpp"
#include "pointer.hpp"
#include "layer.hpp"

#include <thread>

surfaceId surface::allocateSurface(windowId winId, const surfaceSpec& surfaceDataSpec)
{
    
    surfaceId id;
    if(!freeSlots.empty())
    {
        id = {
            .gen = idToIndex[freeSlots.front()].gen,
            .index = (uint16_t)freeSlots.front()
        };
        freeSlots.pop_front();
    }
    else
    {
        id = {
            .gen = 0,
            .index = (uint16_t)idToIndex.size()
        };
        idToIndex.push_back({0});
    }


    surfaceData info;
    info.height = surfaceDataSpec.height;
    info.width = surfaceDataSpec.width;
    info.rule = surfaceDataSpec.rule;
    info.rendererType = surfaceDataSpec.rendererType;
    info.id = id;
    info.parentWindowId = winId;
    info.surface = wl_compositor_create_surface(compositor);

    CONDTION_LOG_FATAL("can't  create surface", info.surface == NULL);
    idToIndex[id.index].surfaceDataIndex = surfaces.size();
    surfaces.push_back(info);


    switch (info.rule)
    {
        case surfaceRule::layer:
            layer::allocateLayer(id, info.surface, surfaceDataSpec);
            break;

        case surfaceRule::topLevel:
            toplevel::allocateTopLevel(id, info.surface, surfaceDataSpec);

            break;
        
        case surfaceRule::subsurface:
            subsurface::allocateSubsurface(id, info.surface, surfaceDataSpec);
            break;
        default:
            LOG_FATAL("need to set valid window rule")
            break;
    }

    switch (info.rendererType)
    {
        case surfaceRenderAPI::openGL:        
            openGLRendering::allocateSurfaceToRender(id);
            break;

        case surfaceRenderAPI::cpu:        
            cpuRendering::allocateSurfaceToRender(id);
            break;
    
        default:
            LOG_FATAL("need to set with render api")
            break;
    }
    
    keyboard::allocateWindowEvents(id);
    pointer::allocateWindowEvents(id);
    wl_surface_commit(info.surface);

    return id;
}

void surface::deallocateSurface(surfaceId winId)
{
    uint8_t index = idToIndex[winId.index].surfaceDataIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == (uint8_t)-1)
        return;


    keyboard::deallocateWindowEvents(winId);
    pointer::deallocateWindowEvents(winId);
    cpuRendering::deallocateSurfaceToRender(winId);

    surfaceData& temp = surfaces[index];
    toplevel::deallocateTopLevel(winId);
    layer::deallocateLayer(winId);
    wl_surface_destroy(temp.surface);

    idToIndex[winId.index].surfaceDataIndex = -1;
    idToIndex[winId.index].gen = -1;
}

void surface::resize(surfaceId id, int width, int height)
{
    uint8_t index = idToIndex[id.index].surfaceDataIndex;
    if(idToIndex[id.index].gen != id.gen || index == (uint8_t)-1)
        return;

    surfaceData& temp = surfaces[index];
    temp.width = width;
    temp.height = height;

    switch (temp.rendererType)
    {
        case surfaceRenderAPI::openGL:        
            openGLRendering::resize(id, width, height);
            break;

        case surfaceRenderAPI::cpu:        
            cpuRendering::resize(id, width, height);
            break;
    
        default:
            LOG_FATAL("need to set with render api")
            break;
    }
}

void surface::setWindowHeight(surfaceId id, int height)
{
    if(idToIndex[id.index].surfaceDataIndex != (uint8_t)-1 && id.gen == idToIndex[id.index].gen)
        resize(id, surfaces[idToIndex[id.index].surfaceDataIndex].width, height);
    
}


int surface::getWindowHeight(surfaceId id)
{
    if(idToIndex[id.index].surfaceDataIndex != (uint8_t)-1 && id.gen == idToIndex[id.index].gen)
        return surfaces[idToIndex[id.index].surfaceDataIndex].height;
        
    return -1;
}

void surface::setWindowWidth(surfaceId id, int width)
{
    if(idToIndex[id.index].surfaceDataIndex != (uint8_t)-1 && id.gen == idToIndex[id.index].gen)
        resize(id, width, surfaces[idToIndex[id.index].surfaceDataIndex].height);
}


int surface::getWindowWidth(surfaceId id)
{
    if(idToIndex[id.index].surfaceDataIndex != (uint8_t)-1 && id.gen == idToIndex[id.index].gen)
        return surfaces[idToIndex[id.index].surfaceDataIndex].width;
        
    return -1;
}

#endif