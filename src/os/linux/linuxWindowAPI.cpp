#ifdef __linux__
#include "linuxWindowAPI.hpp"
#include "log.hpp"

#include <sstream>
#include <utility>

void linuxWindowAPI::global_registry_handler(
            void *data, 
            struct wl_registry *registry, 
            uint32_t id, 
            const char *interface, 
            uint32_t version)
{
    linuxWindowAPI *factory = (linuxWindowAPI*)data;
    // LOG_INFO("Got a registry event for " << interface << " id " << id);

    if (strcmp(interface, "wl_compositor") == 0)
        factory->compositor = (wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    else if (strcmp(interface, "wl_shm") == 0)
            factory->shm = (wl_shm *)wl_registry_bind(registry, id, &wl_shm_interface, 1);
                
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        factory->xdgWmBase = (xdg_wm_base *)wl_registry_bind(registry, id, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(factory->xdgWmBase, &xdg_wm_base_listener, data);
    }
    else if (strcmp(interface, wl_seat_interface.name) == 0) {
        factory->seat = (wl_seat *)wl_registry_bind(registry, id, &wl_seat_interface, 1);
        wl_seat_add_listener(factory->seat, &wl_seat_listener, data);
    }
    else if( strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0 )
    {
        factory->decorationManger = (zxdg_decoration_manager_v1 *)wl_registry_bind(registry, id, &zxdg_decoration_manager_v1_interface, 1);
    }
}

void linuxWindowAPI::toplevelDecorationConfigure(void *data, zxdg_toplevel_decoration_v1 *zxdg_toplevelDecoration, uint32_t mode)
{

}


void linuxWindowAPI::xdgTopLevelConfigure(void *data, xdg_toplevel *xdgToplevel, int32_t width, int32_t height, wl_array *states)
{
    if(width == 0 || height == 0)
        return;

    auto *info = (std::pair<linuxWindowAPI* , windowId>*)data;
    int64_t index = info->first->getIndexFromId(info->second);
    if(index == -1)
        return;

    windowInfo& temp = info->first->windowsInfo[index];

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
    auto *info = (std::pair<linuxWindowAPI* , windowId>*)data;
    info->first->closeWindow(info->second);
}



void linuxWindowAPI::wl_seat_capabilities (void *data, wl_seat *seat, uint32_t serial)
{

}

void linuxWindowAPI::wl_seat_name(void *data, struct wl_seat *seat, const char *name)
{
    LOG_INFO(name);
}


void linuxWindowAPI::global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    LOG_INFO("Got a registry losing event for " << id);
}

void linuxWindowAPI::wl_buffer_release(void *data, struct wl_buffer *wl_buffer)
{
    /* Sent by the compositor when it's no longer using this buffer */
    wl_buffer_destroy(wl_buffer);
}



void linuxWindowAPI::xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial)
{
    auto *info = (std::pair<linuxWindowAPI* , windowId>*)data;
    int64_t index = info->first->getIndexFromId(info->second);
    if(index == -1)
        return;

    windowInfo& temp = info->first->windowsInfo[index];

    xdg_surface_ack_configure(xdg_surface, serial);

    //todo: triger the window on render event chain to all the relvent scenes and get the buffer from that
    
    struct wl_buffer *buffer = draw_frame(temp.width, temp.height);
    
    wl_surface_attach(temp.surface, buffer, 0, 0);
    wl_surface_commit(temp.surface);
}


void linuxWindowAPI::xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

linuxWindowAPI::~linuxWindowAPI()
{
    wl_display_disconnect(display);
}

linuxWindowAPI::linuxWindowAPI()
{
    display = wl_display_connect(NULL);

    CONDTION_LOG_FATAL("can't open window", display == NULL);
    LOG_INFO("connected to display")

    wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, this);

    wl_display_roundtrip(display);

    CONDTION_LOG_FATAL("can't find compositor", compositor == NULL);
    CONDTION_LOG_ERROR("can't find decoration manger", decorationManger == NULL);
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
    xdg_surface_add_listener(info.xdgSurface, &xdg_surface_listener, new std::pair(this, id));
    
    info.xdgToplevel = xdg_surface_get_toplevel(info.xdgSurface);
    xdg_toplevel_add_listener(info.xdgToplevel, &xdgTopLevelListener, new std::pair(this, id));
    xdg_toplevel_set_title(info.xdgToplevel, info.title.c_str());
    
    info.topLevelDecoration = zxdg_decoration_manager_v1_get_toplevel_decoration(decorationManger, info.xdgToplevel);
    zxdg_toplevel_decoration_v1_set_mode(info.topLevelDecoration, 2);
    zxdg_toplevel_decoration_v1_add_listener(info.topLevelDecoration, &toplevelDecorationListener, new std::pair<linuxWindowAPI* , windowId>(this, id));
    
    
    windowsInfo.push_back(info);

    wl_surface_commit(info.surface);
    wl_display_dispatch(display);

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


#endif