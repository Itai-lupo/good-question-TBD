#ifdef __linux__
#include "window.hpp"
#include "cpuRendering.hpp"
#include "linuxWindowAPI.hpp"
#include "log.hpp"

#include <thread>

void window::xdgTopLevelConfigure(void *data, xdg_toplevel *xdgToplevel, int32_t width, int32_t height, wl_array *states)
{
    if(width == 0 || height == 0)
        return;

    windowId id = *(windowId*)data;
    uint8_t index = idToIndex[id.index].windowDataIndex;
    if(index == -1 || id.gen != idToIndex[id.index].gen)
        return;

    windowData& temp = windows[index];
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
    

    cpuRendering::reallocateWindowCpuPool(id);

    index = idToIndex[id.index].resizeEventIndex;
    if(index != -1)
        std::thread(resizeEventListeners[index], windowResizeData{height, width, activeState}).detach();
}

void window::xdgTopLevelClose(void *data, xdg_toplevel *xdgToplevel)
{
    windowId id = *(windowId*)data;

    uint8_t index = idToIndex[id.index].closeEventIndex;
    if(index != -1 || id.gen == idToIndex[id.index].gen)
        closeEventListeners[index]();
        
    linuxWindowAPI::closeWindow(id);
}

void window::xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial)
{
    windowId id = *(windowId*)data;
    uint8_t index = idToIndex[id.index].windowDataIndex;
    if(index == -1 || id.gen != idToIndex[id.index].gen)
        return;

    windowData& temp = windows[index];

    xdg_surface_ack_configure(xdg_surface, serial);
    index = cpuRendering::idToIndex[id.index].renderDataIndex;
    cpuRendering::renderInfo& tempR = cpuRendering::surfacesToRender[index];


    struct wl_buffer *buffer = cpuRendering::allocateWindowBuffer(id, tempR.bufferToRender);
    
    int tempBuffer = tempR.bufferToRender;
    tempR.bufferToRender = tempR.bufferInRender;
    tempR.bufferInRender = tempBuffer;

    wl_surface_attach(temp.surface, buffer, 0, 0);
    wl_surface_commit(temp.surface);
}



void window::allocateWindow(windowId winId, windowSpec winData)
{
    if(winId.index >= idToIndex.size())
        idToIndex.resize(winId.index + 1);

    idToIndex[winId.index].gen = winId.gen;


    windowData info;
    info.height = winData.h;
    info.width = winData.w;
    info.title = winData.title;
    info.id = winId;

    info.surface = wl_compositor_create_surface(compositor);
    
    CONDTION_LOG_FATAL("can't  create surface", info.surface == NULL);


    info.xdgSurface = xdg_wm_base_get_xdg_surface(xdgWmBase, info.surface);
    xdg_surface_add_listener(info.xdgSurface, &xdg_surface_listener, new windowId(winId));
    
    info.xdgToplevel = xdg_surface_get_toplevel(info.xdgSurface);
    xdg_toplevel_add_listener(info.xdgToplevel, &xdgTopLevelListener, new windowId(winId));
    xdg_toplevel_set_title(info.xdgToplevel, info.title.c_str());
    
    
    info.topLevelDecoration = zxdg_decoration_manager_v1_get_toplevel_decoration(decorationManger, info.xdgToplevel);
    zxdg_toplevel_decoration_v1_set_mode(info.topLevelDecoration, 2);
    zxdg_toplevel_decoration_v1_add_listener(info.topLevelDecoration, &toplevelDecorationListener, new windowId(winId));
    
    idToIndex[winId.index].windowDataIndex = windows.size();
    windows.push_back(info);
}

void window::setCloseEventListener(windowId winId, std::function<void()> callback)
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

void window::setResizeEventListener(windowId winId, std::function<void(const windowResizeData&)> callback)
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


void window::deallocateWindow(windowId winId)
{
    uint8_t index = idToIndex[winId.index].windowDataIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;


    windowData& temp = windows[index];

    zxdg_toplevel_decoration_v1_destroy(temp.topLevelDecoration);
    xdg_toplevel_destroy(temp.xdgToplevel);
    xdg_surface_destroy(temp.xdgSurface);
    wl_surface_destroy(temp.surface);

    unsetCloseEventListener(winId);
    unsetResizeEventListener(winId);
    idToIndex[winId.index].windowDataIndex = -1;
    idToIndex[winId.index].gen = -1;
}

void window::unsetCloseEventListener(windowId winId)
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

void window::unsetResizeEventListener(windowId winId)
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

void window::setWindowTitle(windowId id, const std::string& title)
{
    if(idToIndex[id.index].windowDataIndex != -1 && id.gen == idToIndex[id.index].gen)
        windows[idToIndex[id.index].windowDataIndex].title = title;
}


std::string window::getWindowTitle(windowId id)
{
    if(idToIndex[id.index].windowDataIndex != -1 && id.gen == idToIndex[id.index].gen)
        return windows[idToIndex[id.index].windowDataIndex].title;

    return "error window with id: " + std::to_string(id.gen << 8 + id.index) + " was'nt found";
}

void window::setWindowHeight(windowId id, int height)
{
    if(idToIndex[id.index].windowDataIndex != -1 && id.gen == idToIndex[id.index].gen)
        windows[idToIndex[id.index].windowDataIndex].height = height;
}


int window::getWindowHeight(windowId id)
{
    if(idToIndex[id.index].windowDataIndex != -1 && id.gen == idToIndex[id.index].gen)
        return windows[idToIndex[id.index].windowDataIndex].height;
        
    return -1;
}

void window::setWindowTitle(windowId id, int width)
{
    if(idToIndex[id.index].windowDataIndex != -1 && id.gen == idToIndex[id.index].gen)
        windows[idToIndex[id.index].windowDataIndex].width = width;
}


int window::getWindowWidth(windowId id)
{
    if(idToIndex[id.index].windowDataIndex != -1 && id.gen == idToIndex[id.index].gen)
        return windows[idToIndex[id.index].windowDataIndex].width;
        
    return -1;
}

#endif