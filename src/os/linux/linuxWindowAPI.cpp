#ifdef __linux__
#include "linuxWindowAPI.hpp"
#include "log.hpp"
#include "toplevelDecoration.hpp"
#include "seat.hpp"
#include "keyboard.hpp"

#include <sstream>
#include <utility>
#include <sys/prctl.h>

void linuxWindowAPI::global_registry_handler(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    if (strcmp(interface, "wl_compositor") == 0)
        compositor = (wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, 5);
    else if (strcmp(interface, "wl_shm") == 0)
            shm = (wl_shm *)wl_registry_bind(registry, id, &wl_shm_interface, 1);
                
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        xdgWmBase = (xdg_wm_base *)wl_registry_bind(registry, id, &xdg_wm_base_interface, 4);
        xdg_wm_base_add_listener(xdgWmBase, &xdg_wm_base_listener, data);
    }
    else if (strcmp(interface, wl_seat_interface.name) == 0) {
        seat::seatHandle = (wl_seat *)wl_registry_bind(registry, id, &wl_seat_interface, 7);
        wl_seat_add_listener(seat::seatHandle, &seat::wl_seat_listener, data);
    }
    else if(strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0)
    {
        toplevelDecoration::decorationManger = (zxdg_decoration_manager_v1 *)wl_registry_bind(registry, id, &zxdg_decoration_manager_v1_interface, 1);
    }
}

void linuxWindowAPI::wlSurfaceFrameDone(void *data, wl_callback *cb, uint32_t time)
{
    wl_callback_destroy(cb);
    windowId id = *(windowId*)data;
    int64_t index = getIndexFromId(id);
    if(index == -1)
        return;

    windowInfo& temp = windowsInfo[index];
    
    
    cb = wl_surface_frame(temp.surface);
    wl_callback_add_listener(cb, &wlSurfaceFrameListener, data);

    if(temp.lastFrame != 0)
    {
        int elpased = time - temp.lastFrame;
        temp.offset += elpased / 1000.0 * 24;
    }
    struct wl_buffer *buffer = draw_frame(temp.width, temp.height, temp.offset);
    wl_surface_attach(temp.surface, buffer, 0, 0);
    wl_surface_damage_buffer(temp.surface, 0, 0, temp.width, temp.height);
    wl_surface_commit(temp.surface);


    temp.lastFrame = time;
}

void linuxWindowAPI::xdgTopLevelConfigure(void *data, xdg_toplevel *xdgToplevel, int32_t width, int32_t height, wl_array *states)
{
    if(width == 0 || height == 0)
        return;

    windowId id = *(windowId*)data;
    int64_t index = getIndexFromId(id);
    if(index == -1)
        return;

    windowInfo& temp = windowsInfo[index];

    for (size_t i = 0; i < states->size; i+=4)
    {
        if(((uint32_t*)states->data)[i] == 3 )
        {
            temp.height = height;
            temp.width = width;
        }
        else if(((uint32_t*)states->data)[i] == 1 || ((uint32_t*)states->data)[i] == 2)
        {
            temp.height = temp.height > height ? temp.height: height;
            temp.width = temp.width > width ? temp.width: width;

        }
    }    
}

void linuxWindowAPI::xdgTopLevelClose(void *data, xdg_toplevel *xdgToplevel)
{
    windowId id = *(windowId*)data;
    closeWindow(id);
}

void linuxWindowAPI::global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    LOG_INFO("Got a registry losing event for " << id);
}

void linuxWindowAPI::wl_buffer_release(void *data, struct wl_buffer *wl_buffer)
{
    wl_buffer_destroy(wl_buffer);
}

void linuxWindowAPI::xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial)
{
    windowId id = *(windowId*)data;
    int64_t index = getIndexFromId(id);
    if(index == -1)
        return;

    windowInfo& temp = windowsInfo[index];

    xdg_surface_ack_configure(xdg_surface, serial);

    //todo: triger the window on render event chain to all the relvent scenes and get the buffer from that
    struct wl_buffer *buffer = draw_frame(temp.width, temp.height, temp.offset);
    
    wl_surface_attach(temp.surface, buffer, 0, 0);
    wl_surface_commit(temp.surface);

}

void linuxWindowAPI::xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

void linuxWindowAPI::closeApi()
{
    wl_display_disconnect(display);
}

void linuxWindowAPI::windowEventListener()
{
    std::string thradNameA = "Event listener";
    prctl(PR_SET_NAME, thradNameA.c_str());
    
    while (wl_display_dispatch(display)) {}
}

void linuxWindowAPI::init()
{
    display = wl_display_connect(NULL);

    CONDTION_LOG_FATAL("can't open window", display == NULL);
    LOG_INFO("connected to display")

    wl_registry *registry = wl_display_get_registry(display);
    keyboard::xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_roundtrip(display);

    CONDTION_LOG_FATAL("can't find compositor", compositor == NULL);
    CONDTION_LOG_ERROR("can't find decoration manger", toplevelDecoration::decorationManger == NULL);

    eventListenr = new std::thread(windowEventListener);
}

windowId linuxWindowAPI::createWindow(const windowSpec& windowToCreate)
{
    windowInfo info;
    info.height = windowToCreate.h;
    info.width = windowToCreate.w;
    info.title = windowToCreate.title;

    idToIndex[windowsInfo.size()].first = windowsInfo.size();
    windowId id = {
        .gen = idToIndex[windowsInfo.size()].second,
        .index = (uint32_t)windowsInfo.size()
    };
    info.id = id;
    
    info.surface = wl_compositor_create_surface(compositor);
    
    CONDTION_LOG_FATAL("can't  create surface", info.surface == NULL);


    info.xdgSurface = xdg_wm_base_get_xdg_surface(xdgWmBase, info.surface);
    xdg_surface_add_listener(info.xdgSurface, &xdg_surface_listener,new windowId(id));
    
    info.xdgToplevel = xdg_surface_get_toplevel(info.xdgSurface);
    xdg_toplevel_add_listener(info.xdgToplevel, &xdgTopLevelListener,new windowId(id));
    xdg_toplevel_set_title(info.xdgToplevel, info.title.c_str());
    
    
    info.topLevelDecoration = zxdg_decoration_manager_v1_get_toplevel_decoration(toplevelDecoration::decorationManger, info.xdgToplevel);
    zxdg_toplevel_decoration_v1_set_mode(info.topLevelDecoration, 2);
    zxdg_toplevel_decoration_v1_add_listener(info.topLevelDecoration, &toplevelDecoration::toplevelDecorationListener, new windowId(id));
    
    
    windowsInfo.push_back(info);
    wl_surface_commit(info.surface);

    wl_callback *cb = wl_surface_frame(info.surface);
    wl_callback_add_listener(cb, &wlSurfaceFrameListener, new windowId(id));
    
    return id;
}

void linuxWindowAPI::closeWindow(windowId winId)
{
    if(idToIndex.find(winId.index) != idToIndex.end() && idToIndex[winId.index].second != winId.gen)
        return;

    windowInfo temp = windowsInfo[idToIndex[winId.index].first];
    

    idToIndex[winId.index].second++;
    windowInfo last = windowsInfo[windowsInfo.size() - 1];
    windowsInfo[idToIndex[winId.index].first] = last;
    idToIndex[last.id.index].first = idToIndex[winId.index].first;
    
    windowsInfo.pop_back();

    idToIndex[winId.index].first = -1;

    zxdg_toplevel_decoration_v1_destroy(temp.topLevelDecoration);
    xdg_toplevel_destroy(temp.xdgToplevel);
    xdg_surface_destroy(temp.xdgSurface);
    wl_surface_destroy(temp.surface);

}

bool linuxWindowAPI::isWindowOpen(windowId winId)
{
    return getIndexFromId(winId) != -1;
}

std::string linuxWindowAPI::getWindowTitle(windowId winId)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return "";
    return windowsInfo[index].title;

}

std::pair<uint32_t, uint32_t> linuxWindowAPI::getWindowSize(windowId winId)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return {0, 0};

    return {windowsInfo[index].width, windowsInfo[index].height};

}

        

// ################ set event listener ################################################################
void linuxWindowAPI::setKeyPressEventListenrs(windowId winId, std::function<void(const keyData&)> callback)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].keyPressEventListenrs = callback;
}

void linuxWindowAPI::setKeyReleasedEventListenrs(windowId winId, std::function<void(const keyData&)> callback)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].keyReleasedEventListenrs = callback;

}

void linuxWindowAPI::setKeyRepeatEventListenrs(windowId winId, std::function<void(const keyData&)> callback)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].keyRepeatEventListenrs = callback;

}


void linuxWindowAPI::setMouseButtonPressEventListenrs(windowId winId, std::function<void(const mouseButtonData&)> callback)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].mouseButtonPressEventListenrs = callback;

}

void linuxWindowAPI::setMouseButtonReleasedEventListenrs(windowId winId, std::function<void(const mouseButtonData&)> callback)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].mouseButtonReleasedEventListenrs = callback;

}


void linuxWindowAPI::setMouseMovedListenrs(windowId winId, std::function<void(const mouseMoveData&)> callback)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].mouseMovedListenrs = callback;

}

void linuxWindowAPI::setMouseScrollListenrs(windowId winId, std::function<void(const mouseScrollData&)> callback)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].mouseScrollListenrs = callback;

}



// ################ unset event listener ################################################################
void linuxWindowAPI::unsetKeyPressEventListenrs(windowId winId)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].keyPressEventListenrs = {};

}

void linuxWindowAPI::unsetKeyReleasedEventListenrs(windowId winId)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].keyPressEventListenrs = {};

}

void linuxWindowAPI::unsetKeyRepeatEventListenrs(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].keyPressEventListenrs = {};
}


void linuxWindowAPI::unsetMouseButtonPressEventListenrs(windowId winId)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].keyPressEventListenrs = {};
}

void linuxWindowAPI::unsetMouseButtonReleasedEventListenrs(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].keyPressEventListenrs = {};
}


void linuxWindowAPI::unsetMouseMovedListenrs(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].keyPressEventListenrs = {};
}

void linuxWindowAPI::unsetMouseScrollListenrs(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].keyPressEventListenrs = {};
}


#endif