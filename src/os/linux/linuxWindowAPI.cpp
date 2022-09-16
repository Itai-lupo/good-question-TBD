#ifdef __linux__
#include "linuxWindowAPI.hpp"
#include "log.hpp"
#include "seat.hpp"
#include "keyboard.hpp"
#include "pointer.hpp"
#include "window.hpp"
#include "cpuRendering.hpp"

#include <sstream>
#include <utility>
#include <sys/prctl.h>
#include <condition_variable>

void linuxWindowAPI::global_registry_handler(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    // LOG_INFO(interface)
    if (strcmp(interface, "wl_compositor") == 0)
        window::compositor = (wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, 5);
    else if (strcmp(interface, "wl_shm") == 0)
            cpuRendering::shm = (wl_shm *)wl_registry_bind(registry, id, &wl_shm_interface, 1);
                
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        window::xdgWmBase = (xdg_wm_base *)wl_registry_bind(registry, id, &xdg_wm_base_interface, 4);
        xdg_wm_base_add_listener(window::xdgWmBase, &xdg_wm_base_listener, data);
    }
    else if (strcmp(interface, wl_seat_interface.name) == 0) {
        seat::seatHandle = (wl_seat *)wl_registry_bind(registry, id, &wl_seat_interface, 7);
        wl_seat_add_listener(seat::seatHandle, &seat::wl_seat_listener, data);
    }
    else if(strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0)
    {
        window::decorationManger = (zxdg_decoration_manager_v1 *)wl_registry_bind(registry, id, &zxdg_decoration_manager_v1_interface, 1);
    }
}

void linuxWindowAPI::global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    LOG_INFO("Got a registry losing event for " << id);
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

    // CONDTION_LOG_FATAL("can't find compositor", compositor == NULL);
    // CONDTION_LOG_ERROR("can't find decoration manger", decorationManger == NULL);

    eventListenr = new std::thread(windowEventListener);
}

windowId linuxWindowAPI::createWindow(const windowSpec& windowToCreate)
{
    windowId id;
    if(!freeSlots.empty())
    {
        LOG_INFO("slot " << freeSlots.front() << " slot gen " << idToIndex[freeSlots.front()].gen)
        id = {
            .gen = idToIndex[freeSlots.front()].gen,
            .index = (uint8_t)freeSlots.front()
        };
        freeSlots.pop_front();
    }
    else if(hightestId < idToIndex.size()) 
    {
        LOG_INFO("slot " << hightestId << " slot gen " << idToIndex[hightestId].gen)
        id = {
            .gen = idToIndex[hightestId].gen,
            .index = (uint8_t)hightestId
        };
        hightestId++;
    }
    else{
        return {
            .gen    = (uint8_t)-1,
            .index  = (uint8_t)-1
        };
    }

    windowInfo info;
    info.id = id;

    keyboard::allocateWindowEvents(id);
    pointer::allocateWindowEvents(id);
    window::allocateWindow(id, windowToCreate);
    cpuRendering::allocateSurfaceToRender(id);

    idToIndex[id.index].index = windowsInfoSize;
    windowsInfo[windowsInfoSize] = info;
    windowsInfoSize++;

    wl_surface_commit(window::getSurface(id));
    return id;
}

void linuxWindowAPI::closeWindow(windowId winId)
{
    if(winId.gen != idToIndex[winId.index].gen)
        return;

    windowInfo temp = windowsInfo[idToIndex[winId.index].index];
    
    if(winId.index == smallestWindowId)
        smallestWindowId = INT32_MAX;
    keyboard::deallocateWindowEvents(winId);
    pointer::deallocateWindowEvents(winId);
    window::deallocateWindow(winId);

    idToIndex[winId.index].gen++;
    windowInfo last = windowsInfo[windowsInfoSize - 1];
    windowsInfo[idToIndex[winId.index].index] = last;
    idToIndex[last.id.index].index = idToIndex[winId.index].index;
    
    windowsInfoSize--;

    idToIndex[winId.index].index = -1;
    freeSlots.push_back(winId.index);
}

bool linuxWindowAPI::isWindowOpen(windowId winId)
{
    return getIndexFromId(winId) != -1;
}



#endif