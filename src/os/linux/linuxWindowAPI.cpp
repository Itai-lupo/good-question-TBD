#ifdef __linux__
#include "linuxWindowAPI.hpp"
#include "log.hpp"
#include "seat.hpp"
#include "keyboard.hpp"
#include "pointer.hpp"
#include "cpuRendering.hpp"
#include "toplevel.hpp"
#include "subsurface.hpp"
#include "layer.hpp"
#include "openGLRendering.hpp"

#include <sstream>
#include <utility>
#include <sys/prctl.h>
#include <condition_variable>
#include <xdg-decoration-client-protocol.h>

void linuxWindowAPI::global_registry_handler(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    ZoneScoped;
    // LOG_INFO(interface)
    if (strcmp(interface, "wl_compositor") == 0)
        surface::compositor = (wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, 5);
    else if (strcmp(interface, "wl_shm") == 0)
            cpuRendering::shm = (wl_shm *)wl_registry_bind(registry, id, &wl_shm_interface, 1);
                
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        toplevel::xdgWmBase = (xdg_wm_base *)wl_registry_bind(registry, id, &xdg_wm_base_interface, 4);
        xdg_wm_base_add_listener(toplevel::xdgWmBase, &xdg_wm_base_listener, data);
    }
    else if (strcmp(interface, wl_seat_interface.name) == 0) {
        seat::seatHandle = (wl_seat *)wl_registry_bind(registry, id, &wl_seat_interface, 7);
        wl_seat_add_listener(seat::seatHandle, &seat::wl_seat_listener, data);
    }
    else if(strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0)
    {
        toplevel::decorationManger = (zxdg_decoration_manager_v1 *)wl_registry_bind(registry, id, &zxdg_decoration_manager_v1_interface, 1);
    }
    else if(strcmp(interface, wl_subcompositor_interface.name) == 0)
    {
        subsurface::subcompositor = (wl_subcompositor *)wl_registry_bind(registry, id, &wl_subcompositor_interface, 1);
    }
    else if (!strcmp(interface, zwlr_layer_shell_v1_interface.name)) {
        layer::wlrLayerShell = (zwlr_layer_shell_v1 *)wl_registry_bind (registry, id, &zwlr_layer_shell_v1_interface, 1);
    }
}

void linuxWindowAPI::global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    ZoneScoped;
    LOG_INFO("Got a registry losing event for " << id);
}

void linuxWindowAPI::xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    ZoneScoped;
    xdg_wm_base_pong(xdg_wm_base, serial);
}

void linuxWindowAPI::closeApi()
{
    for (auto& win: windowsInfo->getData())
    {
        closeWindow(win.id);
    }
    
    surface::close();
    wl_display_disconnect(display);

    delete windowsInfo;
    delete windowsPool;

}

void linuxWindowAPI::windowEventListener()
{
    std::string thradNameA = "Event listener";
    prctl(PR_SET_NAME, thradNameA.c_str());
    tracy::SetThreadName("Event listener");

    surface::init();
    
    int a = 1;
    while (a != -1) 
    {
        ZoneScoped;
        a = wl_display_dispatch(display);
    }
}

void linuxWindowAPI::init()
{

    display = wl_display_connect(nullptr);

    CONDTION_LOG_FATAL("can't open window: " << display, display == NULL);
    LOG_INFO("connected to display")

    wl_registry *registry = wl_display_get_registry(display);
    keyboard::xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_roundtrip(display);

    CONDTION_LOG_FATAL("can't find compositor", surface::compositor == NULL);
    CONDTION_LOG_ERROR("can't find decoration manger", toplevel::decorationManger == NULL);
    
    windowsPool = new entityPool(255);
    windowsInfo = new windowsInfoComponent(windowsPool);

    eventListenr = new std::thread(windowEventListener);
    while(surface::surfacePool == nullptr)
        std::this_thread::sleep_for(std::chrono::microseconds(1));
}

windowId linuxWindowAPI::createWindow(const windowSpec& windowToCreate)
{
    windowId id = windowsPool->allocEntity();

    windowInfo info;
    info.id = id;

    info.topLevelSurface = surface::allocateSurface(id, {
        surfaceRule::topLevel,
        windowToCreate.renderAPI,
        windowToCreate.w,
        windowToCreate.h,
        .title = windowToCreate.title,
        .gpuRenderFunction = windowToCreate.gpuRenderFunction
    });

    windowsInfo->setComponent(id, info);
    return id;
}

void linuxWindowAPI::closeWindow(windowId winId)
{
    
    windowInfo *temp = windowsInfo->getComponent(winId);
    if(!temp)
        return;

    
    surface::deallocateSurface(temp->topLevelSurface);

    windowsPool->freeEntity(winId);
}

bool linuxWindowAPI::isWindowOpen(windowId winId)
{
    return windowsPool->isIdValid(winId);
}

#endif