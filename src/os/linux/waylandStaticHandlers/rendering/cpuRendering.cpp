#ifdef __linux__

#include "cpuRendering.hpp"
#include "surface.hpp"
#include "log.hpp"
#include "toplevel.hpp"
#include "layer.hpp"

#include <sys/mman.h>
#include <sstream>
#include <sys/prctl.h>
#include <errno.h>
#include <fcntl.h>
#include <utility>
#include <Tracy.hpp>

void cpuRendering::renderWindow(surfaceId win)
{

    ZoneScoped;
    
    uint32_t index = idToIndex[win.index].renderEventIndex;
    if(idToIndex[win.index].gen != win.gen || index == -1)
        return;

    std::string thradNameA = "render " + toplevel::getWindowTitle(win);
    tracy::SetThreadName(thradNameA.c_str());
    prctl(PR_SET_NAME, thradNameA.c_str());
    
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    wl_callback *cb = wl_surface_frame(surface::getSurface(win));

    wl_callback_add_listener(cb, &wlSurfaceFrameListener, new surfaceId(win));
    struct wl_buffer *buffer = allocateWindowBuffer(win, 0);
    wl_surface_attach(surface::getSurface(win), buffer, 0, 0);

    do {
        ZoneScoped;

        std::chrono::duration<double, std::milli> time_span = t2 - t1;
        t1 = std::chrono::high_resolution_clock::now();
        

        int elpased = time_span.count();

        index = idToIndex[win.index].renderDataIndex;
        renderInfo& temp = surfacesToRender[index];

        int stride = surface::getWindowWidth(win) * 4;
        int bufferSize = stride * surface::getWindowHeight(win);
        surfacesToRender[index].bufferSize = bufferSize;

        int bufferIndex = surfacesToRender[index].freeBuffer;

        uint32_t *frameData = mapWindowCpuBuffer(win, surfacesToRender[index].freeBuffer);
        if(frameData)
            renderEventListeners[index](windowRenderData{win, surface::getWindowWidth(win), surface::getWindowHeight(win), frameData, elpased});
        
        
        int ret = munmap(surfacesToRender[index].buffer, surfacesToRender[index].bufferSize);
        CONDTION_LOG_ERROR(ret, ret != 0)

        uint8_t tempBufferIndex = surfacesToRender[index].freeBuffer;
        surfacesToRender[index].freeBuffer = surfacesToRender[index].bufferToRender;
        surfacesToRender[index].bufferToRender = tempBufferIndex;
        
        surfacesToRender[index].renderFinshedBool = true;
        std::shared_lock lk{*surfacesToRender[index].renderMutex.get()};
        surfacesToRender[index].renderFinshed->notify_one();
        
        t2 = std::chrono::high_resolution_clock::now();
        uint32_t index = idToIndex[win.index].renderEventIndex;
    } while (idToIndex[win.index].gen == win.gen && index != -1);
    
}


void cpuRendering::wlSurfaceFrameDone(void *data, wl_callback *cb, uint32_t time)
{
    ZoneScoped;
    wl_callback_destroy(cb);
    surfaceId id = *(surfaceId*)data;
    
    
    uint32_t index = idToIndex[id.index].renderDataIndex;
    if(idToIndex[id.index].gen != id.gen || index == -1)
        return;

    renderInfo& temp = surfacesToRender[index];
    

    cb = wl_surface_frame(surface::getSurface(id));
    wl_callback_add_listener(cb, &wlSurfaceFrameListener, data);
    
    if(idToIndex[id.index].renderEventIndex != (uint8_t)-1)
    {
        std::unique_lock lk2{*temp.renderMutex.get()};
        temp.renderFinshed->wait(lk2, [&](){ return temp.renderFinshedBool;} );
        temp.renderFinshedBool = false;
    }

    struct wl_buffer *buffer = allocateWindowBuffer(id, temp.bufferToRender);
    int tempBuffer = temp.bufferToRender;
    temp.bufferToRender = temp.bufferInRender;
    temp.bufferInRender = tempBuffer;

    wl_surface_attach(surface::getSurface(id), buffer, 0, 0);
    wl_surface_damage_buffer(surface::getSurface(id), 0, 0, surface::getWindowWidth(id), surface::getWindowHeight(id));
    wl_surface_commit(surface::getSurface(id));
    FrameMarkNamed( toplevel::getWindowTitle(id).c_str());

}


void cpuRendering::wl_buffer_release(void *data, struct wl_buffer *wl_buffer)
{
    wl_buffer_destroy(wl_buffer);
}


void cpuRendering::allocateWindowCpuPool(surfaceId winId)
{
    int stride = surface::getWindowWidth(winId)* 4;
    int size = stride * surface::getWindowHeight(winId);
    size += sysconf(_SC_PAGE_SIZE) - size % sysconf(_SC_PAGE_SIZE);
    size *= 3;
    
    renderInfo& info = surfacesToRender[idToIndex[winId.index].renderDataIndex];
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

void cpuRendering::reallocateWindowCpuPool(surfaceId winId)
{
    int64_t index = idToIndex[winId.index].renderDataIndex;
    if(index == -1 || idToIndex[winId.index].gen != winId.gen)
        return;

    renderInfo& info = surfacesToRender[index];
    int stride = surface::getWindowWidth(winId) * 4;
    int size = stride * surface::getWindowHeight(winId);
    size += sysconf(_SC_PAGE_SIZE) - size % sysconf(_SC_PAGE_SIZE);
    size *= 3;

    if(size < info.memoryPoolSize)
        return;

    int ret;
    do {
        ret = ftruncate(info.fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        close(info.fd);
        LOG_ERROR("could not resize because: " << strerror(errno))
        return;
    }
    wl_shm_pool_resize(info.pool, size);
    info.memoryPoolSize = size;
}


wl_buffer *cpuRendering::allocateWindowBuffer(const surfaceId winId, uint32_t offset)
{
    int stride = surface::getWindowWidth(winId) * 4;
    int size = stride * surface::getWindowHeight(winId);
    int temp = size + (sysconf(_SC_PAGE_SIZE) - size % sysconf(_SC_PAGE_SIZE));
    temp *= offset;
    
    
    wl_buffer *buffer = wl_shm_pool_create_buffer(surfacesToRender[idToIndex[winId.index].renderDataIndex].pool, temp, surface::getWindowWidth(winId), surface::getWindowHeight(winId), stride, WL_SHM_FORMAT_ARGB8888);

    wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
    return buffer;
}

uint32_t *cpuRendering::mapWindowCpuBuffer(surfaceId winId, uint32_t offset)
{
    int stride = surface::getWindowWidth(winId) * 4;
    int size = stride * surface::getWindowHeight(winId);
    int temp = size + (sysconf(_SC_PAGE_SIZE) - size % sysconf(_SC_PAGE_SIZE));
    temp *= offset;
    
    renderInfo& info = surfacesToRender[idToIndex[winId.index].renderDataIndex];
    
    uint32_t *data = (uint32_t *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, info.fd, temp);
    
    if (data == MAP_FAILED) {
        LOG_ERROR("couldn't map memory at " << info.fd << " of size " << size << " at offset " << offset << "(" << temp << ")" << " from pool of size " << info.memoryPoolSize << " because: " << strerror(errno))
        close(info.fd);
        return NULL;
    }
    
    info.buffer = data;
    info.bufferSize = size;
    return data;
}


void cpuRendering::randname(char *buf)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long r = ts.tv_nsec;
    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A'+(r&15)+(r&16)*2;
        r >>= 5;
    }
}

int cpuRendering::create_shm_file()
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

int cpuRendering::allocate_shm_file(size_t size)
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


void cpuRendering::allocateSurfaceToRender(surfaceId winId)
{
    if(winId.index >= idToIndex.size())
        idToIndex.resize(winId.index + 1);

    idToIndex[winId.index].gen = winId.gen;

    renderInfo info;
    info.id = winId;

    idToIndex[winId.index].renderDataIndex = surfacesToRender.size();
    surfacesToRender.push_back(info);

    allocateWindowCpuPool(winId);


    wl_callback *cb = wl_surface_frame(surface::getSurface(winId));
    wl_callback_add_listener(cb, &wlSurfaceFrameListener, new surfaceId(winId));
}


void cpuRendering::setRenderEventListeners(surfaceId winId, std::function<void(const windowRenderData&)> callback){
    uint32_t index = idToIndex[winId.index].renderEventIndex;
    if(winId.index >= idToIndex.size() || idToIndex[winId.index].gen != winId.gen)
        return;

    if(index != (uint8_t)-1)
    {    
        renderEventListeners[index] = callback;
        renderEventId[index] = winId;
        return;
    }

    idToIndex[winId.index].renderEventIndex = renderEventListeners.size();
    renderEventListeners.push_back(callback);
    renderEventId.push_back(winId);

    surfacesToRender[idToIndex[winId.index].renderDataIndex].renderThread = new std::thread(renderWindow, winId);

}     



void cpuRendering::deallocateSurfaceToRender(surfaceId winId)
{
    if(idToIndex[winId.index].gen != winId.gen)
        return;

    unsetRenderEventListeners(winId);

    idToIndex[winId.index].renderDataIndex = -1;
    idToIndex[winId.index].gen = -1;

}


void cpuRendering::unsetRenderEventListeners(surfaceId winId)
{

    uint32_t index = idToIndex[winId.index].renderEventIndex;
    if(idToIndex[winId.index].gen != winId.gen || index == -1)
        return;

    uint32_t lastIndex = renderEventListeners.size() - 1;
    idToIndex[renderEventId[lastIndex].index].renderEventIndex = index;
    renderEventListeners[index] = renderEventListeners[lastIndex];
    renderEventId[index] = renderEventId[lastIndex];

    renderEventListeners.pop_back();
    renderEventId.pop_back();

    idToIndex[winId.index].renderEventIndex = -1;
}

void cpuRendering::resize(surfaceId id, int width, int height)
{
    ZoneScoped;

    reallocateWindowCpuPool(id);
    
}

#endif