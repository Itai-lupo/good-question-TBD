#include "toplevel.hpp"
#include "cpuRendering.hpp"
#include "linuxWindowAPI.hpp"

void toplevel::xdgTopLevelConfigure(void *data, xdg_toplevel *xdgToplevel, int32_t width, int32_t height, wl_array *states)
{
    if(width == 0 || height == 0)
        return;

    surfaceId id = *(surfaceId*)data;
    uint8_t index = surface::idToIndex[id.index].surfaceDataIndex;
    if(index == (uint8_t)-1 || id.gen != idToIndex[id.index].gen)
        return;

    surface::surfaceData& temp = surface::surfaces[index];
    windowSizeState activeState = windowSizeState::undefined;

    for (size_t i = 0; i < states->size; i+=4)
    {
        if(((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_RESIZING )
            activeState = windowSizeState::reizing;

        if (((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_ACTIVATED && temp.height != height && temp.width != width)
            activeState = windowSizeState::reizing;

        
        
        if(((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_FULLSCREEN)
            activeState = windowSizeState::fullscreen;
        
        if(((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_MAXIMIZED)
            activeState = windowSizeState::maximized;
        
    }    
    if(activeState == windowSizeState::undefined)
        return;
    temp.height = height;
    temp.width = width;
    LOG_INFO(width << ", " << height)

    cpuRendering::reallocateWindowCpuPool(id);

    index = idToIndex[id.index].resizeEventIndex;
    if(index != (uint8_t)-1)
        std::thread(resizeEventListeners[index], windowResizeData{height, width, activeState}).detach();
}

void toplevel::xdgTopLevelClose(void *data, xdg_toplevel *xdgToplevel)
{
    surfaceId id = *(surfaceId*)data;

    uint8_t index = idToIndex[id.index].closeEventIndex;
    if(index != (uint8_t)-1 || id.gen == idToIndex[id.index].gen)
        closeEventListeners[index]();
        
    linuxWindowAPI::closeWindow(surface::surfaces[surface::idToIndex[id.index].surfaceDataIndex].parentWindowId);
}

void toplevel::xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial)
{
    surfaceId id = *(surfaceId*)data;
    uint8_t index = surface::idToIndex[id.index].surfaceDataIndex;
    if(index == (uint8_t)-1 || id.gen != idToIndex[id.index].gen)
        return;


    xdg_surface_ack_configure(xdg_surface, serial);
    index = cpuRendering::idToIndex[id.index].renderDataIndex;
    cpuRendering::renderInfo& tempR = cpuRendering::surfacesToRender[index];


    struct wl_buffer *buffer = cpuRendering::allocateWindowBuffer(id, tempR.bufferToRender);
    
    int tempBuffer = tempR.bufferToRender;
    tempR.bufferToRender = tempR.bufferInRender;
    tempR.bufferInRender = tempBuffer;

    wl_surface_attach(surface::getSurface(id), buffer, 0, 0);
    wl_surface_commit(surface::getSurface(id));
}




void toplevel::setCloseEventListener(surfaceId winId, std::function<void()> callback)
{
    uint32_t index = idToIndex[winId.index].closeEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        closeEventListeners[index] = callback;
        closeEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].closeEventIndex = closeEventListeners.size();
    closeEventListeners.push_back(callback);
    closeEventId.push_back(winId);
}

void toplevel::setResizeEventListener(surfaceId winId, std::function<void(const windowResizeData&)> callback)
{
    uint32_t index = idToIndex[winId.index].resizeEventIndex;
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        resizeEventListeners[index] = callback;
        resizeEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].resizeEventIndex = resizeEventListeners.size();
    resizeEventListeners.push_back(callback);
    resizeEventId.push_back(winId);
}



void toplevel::unsetCloseEventListener(surfaceId winId)
{
    uint32_t index = idToIndex[winId.index].closeEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = closeEventListeners.size() - 1;
    idToIndex[closeEventId[lastIndex].index].closeEventIndex = index;
    closeEventListeners[index] = closeEventListeners[lastIndex];
    closeEventId[index] = closeEventId[lastIndex];

    closeEventListeners.pop_back();
    closeEventId.pop_back();

    idToIndex[winId.index].closeEventIndex = -1;
}  

void toplevel::unsetResizeEventListener(surfaceId winId)
{
    uint32_t index = idToIndex[winId.index].resizeEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = resizeEventListeners.size() - 1;
    idToIndex[resizeEventId[lastIndex].index].resizeEventIndex = index;
    resizeEventListeners[index] = resizeEventListeners[lastIndex];
    resizeEventId[index] = resizeEventId[lastIndex];

    resizeEventListeners.pop_back();
    resizeEventId.pop_back();

    idToIndex[winId.index].resizeEventIndex = -1;
}

void toplevel::setWindowTitle(surfaceId id, const std::string& title)
{
    if(idToIndex[id.index].toplevelDataIndex != (uint8_t)-1 && id.gen == idToIndex[id.index].gen){
        xdg_toplevel_set_title(topLevelSurfaces[idToIndex[id.index].toplevelDataIndex].xdgToplevel, topLevelSurfaces[idToIndex[id.index].toplevelDataIndex].title.c_str());
        topLevelSurfaces[idToIndex[id.index].toplevelDataIndex].title = title;
    }
}


std::string toplevel::getWindowTitle(surfaceId id)
{
    if(idToIndex[id.index].toplevelDataIndex != (uint8_t)-1 && id.gen == idToIndex[id.index].gen)
        return topLevelSurfaces[idToIndex[id.index].toplevelDataIndex].title;

    return "error window with id: " + std::to_string((id.gen << 8) + id.index) + " was'nt found";
}

void toplevel::allocateTopLevel(surfaceId id, wl_surface *s, const surfaceSpec& surfaceData)
{
    toplevelSurfaceInfo info;
    info.title = surfaceData.title;
    info.id = id;

    info.xdgSurface = xdg_wm_base_get_xdg_surface(xdgWmBase, s);
    xdg_surface_add_listener(info.xdgSurface, &xdg_surface_listener, new surfaceId(id));
    
    info.xdgToplevel = xdg_surface_get_toplevel(info.xdgSurface);
    xdg_toplevel_add_listener(info.xdgToplevel, &xdgTopLevelListener, new surfaceId(id));
    xdg_toplevel_set_title(info.xdgToplevel, info.title.c_str());
    
    
    info.topLevelDecoration = zxdg_decoration_manager_v1_get_toplevel_decoration(decorationManger, info.xdgToplevel);
    zxdg_toplevel_decoration_v1_set_mode(info.topLevelDecoration, 2);
    zxdg_toplevel_decoration_v1_add_listener(info.topLevelDecoration, &toplevelDecorationListener, new surfaceId(id));
    
    idToIndex.push_back({0});
    idToIndex[id.index].gen = id.gen;
    idToIndex[id.index].toplevelDataIndex = topLevelSurfaces.size();
    topLevelSurfaces.push_back(info);
}

void toplevel::deallocateTopLevel(surfaceId winId)
{
    uint8_t index = idToIndex[winId.index].toplevelDataIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == (uint8_t)-1)
        return;


    toplevelSurfaceInfo& temp = topLevelSurfaces[index];

    zxdg_toplevel_decoration_v1_destroy(temp.topLevelDecoration);
    xdg_toplevel_destroy(temp.xdgToplevel);
    xdg_surface_destroy(temp.xdgSurface);

    unsetCloseEventListener(winId);
    unsetResizeEventListener(winId);
}
