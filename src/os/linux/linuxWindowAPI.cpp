#ifdef __linux__
#include "linuxWindowAPI.hpp"
#include "log.hpp"

void linuxWindowAPI::global_registry_handler(
            void *data, 
            struct wl_registry *registry, 
            uint32_t id, 
            const char *interface, 
            uint32_t version)
{
    linuxWindowAPI *factory = (linuxWindowAPI*)data;
    LOG_INFO("Got a registry event for " << interface << " id " << id);

    if (strcmp(interface, "wl_compositor") == 0)
        factory->compositor = (wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    else if (strcmp(interface, "wl_shm") == 0)
            factory->shm = (wl_shm *)wl_registry_bind(registry, id, &wl_shm_interface, 1);
                
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        factory->xdgWmBase = (xdg_wm_base *)wl_registry_bind(registry, id, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(factory->xdgWmBase, &xdg_wm_base_listener, data);
    }
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
    
    linuxWindow *windowInfo = (linuxWindow *)data;

    xdg_surface_ack_configure(xdg_surface, serial);

    //todo: triger the window on render event chain to all the relvent scenes and get the buffer from that
    struct wl_buffer *buffer = draw_frame(windowInfo->w, windowInfo->h);
    wl_surface_attach(windowInfo->surface, buffer, 0, 0);
    wl_surface_commit(windowInfo->surface);
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
    LOG_INFO("Found compositor")
}

uint64_t linuxWindowAPI::createWindow(windowSpec *windowToCreate)
{
    linuxWindow *windowInfo = new linuxWindow();
    windowInfo->h = windowToCreate->h;
    windowInfo->w = windowToCreate->w;
    windowInfo->title = windowToCreate->title;
    windowInfo->surface = wl_compositor_create_surface(compositor);

    CONDTION_LOG_FATAL("can't  create surface", windowInfo->surface == NULL);
    LOG_INFO("Created surface")
    
    
    windowInfo->xdgSurface = xdg_wm_base_get_xdg_surface(xdgWmBase, windowInfo->surface);
    xdg_surface_add_listener(windowInfo->xdgSurface, &xdg_surface_listener, windowInfo);
    windowInfo->xdgToplevel = xdg_surface_get_toplevel(windowInfo->xdgSurface);
    xdg_toplevel_set_title(windowInfo->xdgToplevel, windowInfo->title.c_str());
    xdg_surface_set_window_geometry(windowInfo->xdgSurface, 0, 0, windowInfo->w, windowInfo->h);
    wl_surface_commit(windowInfo->surface);
    wl_display_dispatch(display);
    return 0;
}


#endif