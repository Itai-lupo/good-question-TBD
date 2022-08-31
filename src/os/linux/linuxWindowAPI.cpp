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
    // LOG_INFO(interface)
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

void linuxWindowAPI::global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    LOG_INFO("Got a registry losing event for " << id);
}

void linuxWindowAPI::wlSurfaceFrameDone(void *data, wl_callback *cb, uint32_t time)
{
    ZoneScoped;
    wl_callback_destroy(cb);
    windowId id = *(windowId*)data;
    int64_t index = getIndexFromId(id);
    if(index == -1)
        return;

    windowInfo& temp = windowsInfo[index];
    
    if(temp.id.index <= smallestWindowId){
        smallestWindowId = temp.id.index; 
        FrameMark;
    }
    
    cb = wl_surface_frame(temp.surface);
    wl_callback_add_listener(cb, &wlSurfaceFrameListener, data);
    
    int elpased = 0;
    if(temp.lastFrame != 0)
        elpased = time - temp.lastFrame;

    temp.bufferOffset = 1 - temp.bufferOffset;
    temp.bufferOffset = 0;
    if(temp.renderThread && temp.renderThread->joinable())
    {
            temp.renderThread->join();  

            int stride = temp.width * 4;
            int bufferSize = stride * temp.height;
            munmap(temp.buffer, temp.bufferSize);
    }

    uint32_t *frameData = mapWindowCpuBuffer(temp);
    if(temp.renderListeners && frameData != NULL)
        temp.renderThread = new std::thread(temp.renderListeners, windowRenderData{temp.width, temp.height, frameData, elpased});
    
    struct wl_buffer *buffer = allocateWindowBuffer(temp);
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
    
    temp.height = height;
    temp.width = width;
    
    if(temp.renderThread && temp.renderThread->joinable() && activeState != windowSizeState::undefined)
    {
        temp.renderThread->join();  

        int stride = temp.width * 4;
        int bufferSize = stride * temp.height;
        munmap(temp.buffer, temp.bufferSize);
        reallocateWindowCpuPool(temp);
    }

    if(temp.resizeListenrs && activeState != windowSizeState::undefined)
        std::thread(temp.resizeListenrs, windowResizeData{height, width, activeState}).detach();
}

void linuxWindowAPI::xdgTopLevelClose(void *data, xdg_toplevel *xdgToplevel)
{
    windowId id = *(windowId*)data;
    int64_t index = getIndexFromId(id);
    if(index == -1)
        return;

    windowInfo& temp = windowsInfo[index];
    if(temp.closeListenrs)
        temp.closeListenrs();

    closeWindow(id);
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

    uint32_t *frameData = mapWindowCpuBuffer(temp);
    
    temp.renderListeners(windowRenderData{temp.width, temp.height, frameData});
    struct wl_buffer *buffer = allocateWindowBuffer(temp);
    temp.renderListeners(windowRenderData{temp.width, temp.height, frameData});

    int stride = temp.width * 4;
    int bufferSize = stride * temp.height;
    munmap(frameData, bufferSize);
    
    wl_surface_attach(temp.surface, buffer, 0, 0);
    wl_surface_commit(temp.surface);

    temp.bufferOffset = 0;
    frameData = mapWindowCpuBuffer(temp);
    if(temp.renderListeners && frameData)
        temp.renderThread = new std::thread(temp.renderListeners, windowRenderData{temp.width, temp.height, frameData});
    
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
    
    allocateWindowCpuPool(info);

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
    
    if(winId.index == smallestWindowId)
        smallestWindowId = INT32_MAX;

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

void linuxWindowAPI::setCloseEventeListenrs(windowId winId, std::function<void()> callback){
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].closeListenrs = callback;

}

void linuxWindowAPI::setResizeEventeListenrs(windowId winId, std::function<void(const windowResizeData&)> callback){
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].resizeListenrs = callback;

}

void linuxWindowAPI::setGainFocusEventListeners(windowId winId, std::function<void()> callback){
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].gainFocusListeners = callback;

}

void linuxWindowAPI::setLostFocusEventListeners(windowId winId, std::function<void()> callback){
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].lostFocusListeners = callback;

}

void linuxWindowAPI::setRenderEventListeners(windowId winId, std::function<void(const windowRenderData&)> callback){
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].renderListeners = callback;

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

    windowsInfo[index].keyReleasedEventListenrs = {};

}

void linuxWindowAPI::unsetKeyRepeatEventListenrs(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].keyRepeatEventListenrs = {};
}


void linuxWindowAPI::unsetMouseButtonPressEventListenrs(windowId winId)
{
    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].mouseButtonPressEventListenrs = {};
}

void linuxWindowAPI::unsetMouseButtonReleasedEventListenrs(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].mouseButtonReleasedEventListenrs = {};
}


void linuxWindowAPI::unsetMouseMovedListenrs(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].mouseMovedListenrs = {};
}

void linuxWindowAPI::unsetMouseScrollListenrs(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].mouseScrollListenrs = {};
}

void linuxWindowAPI::unsetCloseEventeListenrs(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].closeListenrs = {};
}

void linuxWindowAPI::unsetResizeEventeListenrs(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].resizeListenrs = {};
}

void linuxWindowAPI::unsetGainFocusEventListeners(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].gainFocusListeners = {};
}

void linuxWindowAPI::unsetLostFocusEventListeners(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].lostFocusListeners = {};
}

void linuxWindowAPI::unsetRenderEventListeners(windowId winId)
{

    int64_t index = getIndexFromId(winId);
    if(index == -1)
        return;

    windowsInfo[index].renderListeners = {};
}


void linuxWindowAPI::allocateWindowCpuPool(windowInfo& info)
{
    int stride = info.width * 4;
    int size = stride * info.height;

    info.fd = allocate_shm_file(size);
    if (info.fd == -1) {
        return;
    }

    uint32_t *data = (uint32_t *)mmap(NULL, size,
            PROT_READ | PROT_WRITE, MAP_SHARED, info.fd, 0);
    if (data == MAP_FAILED) {
        LOG_ERROR("could not get data")

        close(info.fd);
        return ;
    }

    info.pool = wl_shm_create_pool(shm, info.fd, size);
    info.memoryPoolSize = size;
}

void linuxWindowAPI::reallocateWindowCpuPool(windowInfo& info)
{
    int stride = info.width * 4;
    int size = stride * info.height;

    if(size < info.memoryPoolSize)
        return;

    int ret;
    do {
        ret = ftruncate(info.fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        close(info.fd);
        LOG_ERROR("could not resize")
    }
    wl_shm_pool_resize(info.pool, size);
    info.memoryPoolSize = size;
}


wl_buffer *linuxWindowAPI::allocateWindowBuffer(const windowInfo& info)
{
    int stride = info.width * 4;
    int size = stride * info.height;
    wl_buffer *buffer = wl_shm_pool_create_buffer(info.pool, 0, info.width, info.height, stride, WL_SHM_FORMAT_ARGB8888);

    wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
    return buffer;
}

uint32_t *linuxWindowAPI::mapWindowCpuBuffer(windowInfo& info)
{
    int stride = info.width * 4;
    int size = stride * info.height;

    uint32_t *data = (uint32_t *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, info.fd, 0);
    
    if (data == MAP_FAILED) {
        close(info.fd);
        LOG_ERROR("couldn't map memory")
        return NULL;
    }
    info.buffer = data;
    info.bufferSize = size;
    return data;
}


void linuxWindowAPI::randname(char *buf)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long r = ts.tv_nsec;
    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A'+(r&15)+(r&16)*2;
        r >>= 5;
    }
}

int linuxWindowAPI::create_shm_file()
{
    int retries = 100;
    do {
        char name[] = "/wl_shm-XXXXXX";
        randname(name + sizeof(name) - 7);
        --retries;
        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while (retries > 0 && errno == EEXIST);
    return -1;
}

int linuxWindowAPI::allocate_shm_file(size_t size)
{
    int fd = create_shm_file();
    if (fd < 0)
        return -1;
    int ret;
    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

#endif