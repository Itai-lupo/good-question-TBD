#include "layer.hpp"
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
    
    surfaceData *temp = surface::surfacesInfo->getComponent(id);
    if(!temp)
        return;

    windowSizeState activeState = windowSizeState::undefined;

    temp->height = height;
    temp->width = width;

    surface::resize(id, width, height);
    void(*tempCallback)(const windowResizeData&) =  resizeCallbacks->getCallback(id);
    if(tempCallback)
        std::thread(*tempCallback, windowResizeData{id, (int)height, (int)width, activeState}).detach();
}

void layer::layerClose(void *data, zwlr_layer_surface_v1 *wlrLayer)
{
    surfaceId id = *(surfaceId*)data;
    surfaceCallback temp =  closeCallbacks->getCallback(id);
    
    if(temp)
        temp(id);
        
    linuxWindowAPI::closeWindow(surface::surfacesInfo->getComponent(id)->parentWindowId);
}



void layer::setCloseEventListener(surfaceId winId, void(*callback)(surfaceId id) )
{
    closeCallbacks->setCallback(winId, callback);
}

void layer::setResizeEventListener(surfaceId winId, void(*callback)(const windowResizeData&) )
{
    resizeCallbacks->setCallback(winId, callback);

}

void layer::unsetCloseEventListener(surfaceId winId)
{
    closeCallbacks->deleteComponent(winId);
}  

void layer::unsetResizeEventListener(surfaceId winId)
{
    resizeCallbacks->deleteComponent(winId);
}

void layer::setWindowTitle(surfaceId id, const std::string& title)
{
    layerSurfaceInfo *temp = layers->getComponent(id);

    if(temp)
        temp->title = title;
}


std::string layer::getWindowTitle(surfaceId id)
{
    layerSurfaceInfo *temp = layers->getComponent(id);

    if(temp)
        return temp->title;

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
    
    layers->setComponent(id, info);
}

void layer::deallocateLayer(surfaceId winId)
{
    layers->deleteComponent(winId);
    closeCallbacks->deleteComponent(winId);
    resizeCallbacks->deleteComponent(winId);
}
