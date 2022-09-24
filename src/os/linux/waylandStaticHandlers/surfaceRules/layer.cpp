#include "layer.hpp"
#include "cpuRendering.hpp"
#include "linuxWindowAPI.hpp"
#include <EGL/egl.h>
#include <wayland-egl.h>

void layer::layerConfigure(void *data, zwlr_layer_surface_v1 *wlrLayer, uint32_t serial, uint32_t width, uint32_t height)
{
    LOG_INFO(width << ", " << height)
    zwlr_layer_surface_v1_ack_configure(wlrLayer, serial);
    if(width == 0 || height == 0)
        return;

    surfaceId id = *(surfaceId*)data;
    uint8_t index = surface::idToIndex[id.index].surfaceDataIndex;
    if(index == (uint8_t)-1 || id.gen != idToIndex[id.index].gen)
        return;

    surface::surfaceData& temp = surface::surfaces[index];
    windowSizeState activeState = windowSizeState::undefined;

    temp.height = height;
    temp.width = width;
    cpuRendering::reallocateWindowCpuPool(id);

    index = idToIndex[id.index].resizeEventIndex;
    if(index != (uint8_t)-1)
        std::thread(resizeEventListeners[index], windowResizeData{(int)height, (int)width, activeState}).detach();
    
    
    index = cpuRendering::idToIndex[id.index].renderDataIndex;
    cpuRendering::renderInfo& tempR = cpuRendering::surfacesToRender[index];


    struct wl_buffer *buffer = cpuRendering::allocateWindowBuffer(id, tempR.bufferToRender);
    
    int tempBuffer = tempR.bufferToRender;
    tempR.bufferToRender = tempR.bufferInRender;
    tempR.bufferInRender = tempBuffer;

    wl_surface_attach(surface::getSurface(id), buffer, 0, 0);
    wl_surface_commit(surface::getSurface(id));

    
    index = idToIndex[id.index].layerDataIndex;
    layerSurfaces[index].canRender = true;
}

void layer::layerClose(void *data, zwlr_layer_surface_v1 *wlrLayer)
{
    surfaceId id = *(surfaceId*)data;

    uint8_t index = idToIndex[id.index].closeEventIndex;
    if(index != (uint8_t)-1 || id.gen == idToIndex[id.index].gen)
        closeEventListeners[index]();
        
    linuxWindowAPI::closeWindow(surface::surfaces[surface::idToIndex[id.index].surfaceDataIndex].parentWindowId);
}



void layer::setCloseEventListener(surfaceId winId, std::function<void()> callback)
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

void layer::setResizeEventListener(surfaceId winId, std::function<void(const windowResizeData&)> callback)
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



void layer::unsetCloseEventListener(surfaceId winId)
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

void layer::unsetResizeEventListener(surfaceId winId)
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

void layer::setWindowTitle(surfaceId id, const std::string& title)
{
    if(idToIndex[id.index].layerDataIndex != (uint8_t)-1 && id.gen == idToIndex[id.index].gen){
        // xdg_toplevel_set_title(layerSurfaces[idToIndex[id.index].layerDataIndex].layerSurface, layerSurfaces[idToIndex[id.index].layerDataIndex].title.c_str());
        layerSurfaces[idToIndex[id.index].layerDataIndex].title = title;
    }
}


std::string layer::getWindowTitle(surfaceId id)
{
    if(idToIndex[id.index].layerDataIndex != (uint8_t)-1 && id.gen == idToIndex[id.index].gen)
        return layerSurfaces[idToIndex[id.index].layerDataIndex].title;

    return "error window with id: " + std::to_string((id.gen << 8) + id.index) + " was'nt found";
}

void layer::allocateLayer(surfaceId id, wl_surface *s, const surfaceSpec& surfaceData)
{
    layerSurfaceInfo info;
    info.title = surfaceData.title;
    info.id = id;

    info.layerSurface = zwlr_layer_shell_v1_get_layer_surface(wlrLayerShell, s, NULL, ZWLR_LAYER_SHELL_V1_LAYER_TOP, surfaceData.title.c_str());
    zwlr_layer_surface_v1_set_size(info.layerSurface, surfaceData.width, surfaceData.height);
    zwlr_layer_surface_v1_add_listener(info.layerSurface, &layerSurfaceListener, new surfaceId(id));
    
    
    idToIndex.push_back({0});
    idToIndex[id.index].gen = id.gen;
    idToIndex[id.index].layerDataIndex = layerSurfaces.size();
    layerSurfaces.push_back(info);
}

void layer::deallocateLayer(surfaceId winId)
{
    uint8_t index = idToIndex[winId.index].layerDataIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == (uint8_t)-1)
        return;


    layerSurfaceInfo& temp = layerSurfaces[index];
    zwlr_layer_surface_v1_destroy(temp.layerSurface);

    unsetCloseEventListener(winId);
    unsetResizeEventListener(winId);
}
