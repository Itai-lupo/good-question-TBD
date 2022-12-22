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


void cpuRendering::init(entityPool *surfacesPool)
{
    renderData = new cpuRenderInfoComponent(surfacesPool);
}

void cpuRendering::closeRenderer()
{
    delete renderData;
}



void cpuRendering::allocateSurfaceToRender(surfaceId winId, void(*callback)(const cpuRenderData&))
{    
    cpuRenderInfo info(callback);
    info.id = winId;

    renderData->setComponent(winId, info);
    
    allocateWindowCpuPool(winId);


    wl_callback *cb = wl_surface_frame(surface::getSurface(winId));
    wl_callback_add_listener(cb, &wlSurfaceFrameListener, new surfaceId(winId));


    renderData->getComponent(winId)->renderThread = new std::thread(renderWindow, winId);
}

void cpuRendering::deallocateSurfaceToRender(surfaceId winId)
{
    if(renderData->getComponent(winId)->renderThread->joinable())
        renderData->getComponent(winId)->renderThread->join();
    renderData->deleteComponent(winId);
}



void cpuRendering::setRenderEventListeners(surfaceId winId, void(*callback)(const cpuRenderData&))
{
    renderData->getComponent(winId)->renderFuncion = callback;
}     


void cpuRendering::resize(surfaceId id, int width, int height)
{
    ZoneScoped;

    reallocateWindowCpuPool(id);   
}



void cpuRendering::renderWindow(surfaceId win)
{
    ZoneScoped;   
    
    cpuRenderInfo *info = renderData->getComponent(win);
    if(!info)
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
        int stride = surface::getWindowWidth(win) * 4;
        int bufferSize = stride * surface::getWindowHeight(win);
        
        info->bufferSize = bufferSize;
        int bufferIndex = info->freeBuffer;

        uint32_t *frameData = mapWindowCpuBuffer(win, info->freeBuffer);
        if(frameData)
            info->renderFuncion(cpuRenderData{win, surface::getWindowWidth(win), surface::getWindowHeight(win), frameData, elpased});
        
        
        int ret = munmap(info->buffer, info->bufferSize);
        CONDTION_LOG_ERROR(ret, ret != 0)

        uint8_t tempBufferIndex = info->freeBuffer;
        info->freeBuffer = info->bufferToRender;
        info->bufferToRender = tempBufferIndex;        
        info->renderFinshedBool = true;
        
        t2 = std::chrono::high_resolution_clock::now();    

        info = renderData->getComponent(win);
    } while (info);   
}



// wayland callbacks
void cpuRendering::wlSurfaceFrameDone(void *data, wl_callback *cb, uint32_t time)
{
    ZoneScoped;
    wl_callback_destroy(cb);
    surfaceId id = *(surfaceId*)data;
    
    cpuRenderInfo *info = renderData->getComponent(id);
    if(!info)
        return;
    

    int i = 0;
    for (i = 0; !info->renderFinshedBool && i < 10; i++)
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    
    
    

    struct wl_buffer *buffer = allocateWindowBuffer(id, info->bufferToRender);
    
    if(info->renderFinshedBool)
    {
        int tempBuffer = info->bufferToRender;
        info->bufferToRender = info->bufferInRender;
        info->bufferInRender = tempBuffer;
    }
    info->renderFinshedBool = false;

    wl_surface_attach(surface::getSurface(id), buffer, 0, 0);
    wl_surface_damage_buffer(surface::getSurface(id), 0, 0, surface::getWindowWidth(id), surface::getWindowHeight(id));
    wl_surface_commit(surface::getSurface(id));

    cb = wl_surface_frame(surface::getSurface(id));
    wl_callback_add_listener(cb, &wlSurfaceFrameListener, data);
    
    
#ifdef TRACY_ENABLE
    TracyMessage((std::string("wait ticks count: ") + std::to_string(i)).c_str(), (std::string("wait ticks count: ") + std::to_string(i)).size());
    ZoneText(toplevel::getWindowTitle(id).c_str(), toplevel::getWindowTitle(id).size());
    if(&renderData->getData().back() == info){
        FrameMarkNamed("cpuRendering");
    }
#endif
}

void cpuRendering::wl_buffer_release(void *data, struct wl_buffer *wl_buffer)
{
    wl_buffer_destroy(wl_buffer);
}


// memory mangment helper function
void cpuRendering::allocateWindowCpuPool(surfaceId winId)
{
    int size = caluclateBufferSize(winId);
    size *= 3;
    
    cpuRenderInfo *info = renderData->getComponent(winId);
    if(!info)
        return;

    info->fd = allocateShmFile(size);
    if (info->fd == -1)
        return;

    uint32_t *data = (uint32_t *)mmap(NULL, size,
            PROT_READ | PROT_WRITE, MAP_SHARED, info->fd, 0);
    if (data == MAP_FAILED) {
        LOG_ERROR("could not get data")

        close(info->fd);
        return ;
    }

    info->pool = wl_shm_create_pool(shm, info->fd, size);
    info->memoryPoolSize = size;
}

void cpuRendering::reallocateWindowCpuPool(surfaceId winId)
{
    cpuRenderInfo *info = renderData->getComponent(winId);
    if(!info)
        return;

    int size = caluclateBufferSize(winId);
    size *= 3;

    if(size < info->memoryPoolSize)
        return;

    int ret;
    do {
        ret = ftruncate(info->fd, size);
    } while (ret < 0 && errno == EINTR);

    if (ret < 0) {
        close(info->fd);
        LOG_ERROR("could not resize because: " << strerror(errno))
        return;
    }

    wl_shm_pool_resize(info->pool, size);
    info->memoryPoolSize = size;
}


wl_buffer *cpuRendering::allocateWindowBuffer(const surfaceId winId, uint32_t offset)
{
    int stride = surface::getWindowWidth(winId) * 4;
    int temp = caluclateBufferSize(winId);
    temp *= offset;    
    
    wl_buffer *buffer = wl_shm_pool_create_buffer(renderData->getComponent(winId)->pool, temp, surface::getWindowWidth(winId), surface::getWindowHeight(winId), stride, WL_SHM_FORMAT_ARGB8888);

    wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
    return buffer;
}


uint32_t *cpuRendering::mapWindowCpuBuffer(surfaceId winId, uint32_t offset)
{
    int stride = surface::getWindowWidth(winId) * 4;
    int size = stride * surface::getWindowHeight(winId);
    int temp = caluclateBufferSize(winId) * offset;
    
    cpuRenderInfo *info = renderData->getComponent(winId);
    uint32_t *data = (uint32_t *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, info->fd, temp);
    
    if (data == MAP_FAILED) {
        LOG_ERROR("couldn't map memory at " << info->fd << " of size " << size << " at offset " << offset << "(" << temp << ")" << " from pool of size " << info->memoryPoolSize << " because: " << strerror(errno))
        close(info->fd);
        return NULL;
    }
    
    info->buffer = data;
    info->bufferSize = size;
    return data;
}



int cpuRendering::caluclateBufferSize(surfaceId winId)
{
    int stride = surface::getWindowWidth(winId) * 4;
    int size = stride * surface::getWindowHeight(winId);
    size += sysconf(_SC_PAGE_SIZE) - size % sysconf(_SC_PAGE_SIZE);
    return size;
}

int cpuRendering::allocateShmFile(size_t size)
{
    int fd = createShmFile();
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

int cpuRendering::createShmFile()
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


#endif