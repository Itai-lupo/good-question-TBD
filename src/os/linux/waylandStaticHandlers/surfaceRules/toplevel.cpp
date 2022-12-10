#include "toplevel.hpp"
#include "linuxWindowAPI.hpp"
#include "openGLRendering.hpp"





void toplevel::xdgTopLevelConfigure(void *data, xdg_toplevel *xdgToplevel, int32_t width, int32_t height, wl_array *states)
{
    ZoneScoped;
    
    surfaceId id = *(surfaceId*)data;
    
    toplevelSurfaceInfo *temp = topLevelSurfaces->getComponent(id);

    if(!temp)
        return;

    windowSizeState activeState = windowSizeState::undefined;

    for (size_t i = 0; i < states->size; i+=4)
    {
        if(((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_RESIZING )
            activeState = windowSizeState::reizing;

        if (((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_ACTIVATED && temp->height != height && temp->width != width)
            activeState = windowSizeState::reizing;

        
        
        if(((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_FULLSCREEN)
            activeState = windowSizeState::fullscreen;
        
        if(((uint32_t*)states->data)[i] == xdg_toplevel_state::XDG_TOPLEVEL_STATE_MAXIMIZED)
            activeState = windowSizeState::maximized;
        
    }   
    
    if(activeState == windowSizeState::undefined)
        return;

    temp->width = width;
    temp->height = height;

    resizeCallback tempCallback = resizeCallbacks->getCallback(id);

    if(tempCallback)
        std::thread(tempCallback, windowResizeData{id, height, width, activeState}).detach();
}

void toplevel::xdgTopLevelClose(void *data, xdg_toplevel *xdgToplevel)
{
    ZoneScoped;

    surfaceId id = *(surfaceId*)data;

    closeCallback temp = closeCallbacks->getCallback(id);
    if(temp)
        temp(id);
        
    linuxWindowAPI::closeWindow(surface::surfacesInfo->getComponent(id)->parentWindowId);
}

void toplevel::xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial)
{
    ZoneScoped;
    xdg_surface_ack_configure(xdg_surface, serial);


    surfaceId id = *(surfaceId*)data;

    toplevelSurfaceInfo *temp = topLevelSurfaces->getComponent(id);
    if(!temp)
        return;

    surface::resize(id, temp->width, temp->height);

    wl_surface_commit(surface::getSurface(id));
}




void toplevel::setCloseEventListener(surfaceId winId, void(*callback)(surfaceId winId))
{
    closeCallbacks->setCallback(winId, callback);

}

void toplevel::setResizeEventListener(surfaceId winId, void(*callback)(const windowResizeData&))
{
    resizeCallbacks->setCallback(winId, callback);
}



void toplevel::unsetCloseEventListener(surfaceId winId)
{        
    closeCallbacks->deleteComponent(winId);
}

void toplevel::unsetResizeEventListener(surfaceId winId)
{        
    resizeCallbacks->deleteComponent(winId);
}

void toplevel::setWindowTitle(surfaceId id, const std::string& title)
{        
    toplevelSurfaceInfo *temp = topLevelSurfaces->getComponent(id);

    if(temp){
        xdg_toplevel_set_title(temp->xdgToplevel, temp->title.c_str());
        temp->title = title;
    }
}


std::string toplevel::getWindowTitle(surfaceId id)
{
    toplevelSurfaceInfo *temp = topLevelSurfaces->getComponent(id);

    if(temp)
        return temp->title;

    return "error window with id: " + std::to_string((id.gen << 8) + id.index) + " was'nt found";
}

void toplevel::allocateTopLevel(surfaceId id, wl_surface *s, const surfaceSpec& surfaceData)
{
    toplevelSurfaceInfo info;
    info.title = surfaceData.title;
    info.id = id;
    info.width = surfaceData.width;
    info.height = surfaceData.height;

    info.xdgSurface = xdg_wm_base_get_xdg_surface(xdgWmBase, s);
    xdg_surface_add_listener(info.xdgSurface, &xdg_surface_listener, new surfaceId(id));
    
    info.xdgToplevel = xdg_surface_get_toplevel(info.xdgSurface);
    xdg_toplevel_add_listener(info.xdgToplevel, &xdgTopLevelListener, new surfaceId(id));
    xdg_toplevel_set_title(info.xdgToplevel, info.title.c_str());
    xdg_toplevel_set_app_id(info.xdgToplevel, "new tale game engine");
    
    info.topLevelDecoration = zxdg_decoration_manager_v1_get_toplevel_decoration(decorationManger, info.xdgToplevel);
    zxdg_toplevel_decoration_v1_set_mode(info.topLevelDecoration, 2);
    zxdg_toplevel_decoration_v1_add_listener(info.topLevelDecoration, &toplevelDecorationListener, new surfaceId(id));
    
    topLevelSurfaces->setComponent(id, info);
}

void toplevel::deallocateTopLevel(surfaceId winId)
{
    toplevelSurfaceInfo *temp = topLevelSurfaces->getComponent(winId);
    if(!temp)
        return;

    unsetCloseEventListener(winId);
    unsetResizeEventListener(winId);
}
