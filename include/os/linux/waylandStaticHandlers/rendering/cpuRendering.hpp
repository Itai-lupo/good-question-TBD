#pragma once
#include "core.hpp"
#include "cpuRenderData.hpp"
#include "surface.hpp"

#include "entityPool.hpp"
#include "cpuRenderInfoComponent.hpp"

#include <thread>
#include <memory>
#include <shared_mutex>
#include <condition_variable>

#include <wayland-client.h>

class cpuRendering
{
    private:
		static void wl_buffer_release(void *data, struct wl_buffer *wl_buffer);
        static constexpr struct wl_buffer_listener wl_buffer_listener = {
            .release = wl_buffer_release,
        };

        static void wlSurfaceFrameDone(void *data, wl_callback *cb, uint32_t time);
        static constexpr wl_callback_listener wlSurfaceFrameListener = 
        {
            .done = wlSurfaceFrameDone
        };

        static inline cpuRenderInfoComponent *renderData;
        
        static void renderWindow(surfaceId win); 
        
        static void allocateWindowCpuPool(surfaceId winId);
        static void reallocateWindowCpuPool(surfaceId winId);
        
        static wl_buffer *allocateWindowBuffer(const surfaceId winId, uint32_t offset);
        static uint32_t *mapWindowCpuBuffer(surfaceId winId, uint32_t offset);
        
        static int caluclateBufferSize(surfaceId winId);
        static int allocateShmFile(size_t size);
        static int createShmFile(void);
        static void randname(char *buf);
    
    public:
        static inline wl_shm *shm;

        static void init(entityPool *surfacesPool);
        static void closeRenderer();

        static void allocateSurfaceToRender(surfaceId winId, void(*callback)(const cpuRenderData&));
        static void deallocateSurfaceToRender(surfaceId winId);
    
        static void setRenderEventListeners(surfaceId winId, void(*callback)(const cpuRenderData&));

        static void resize(surfaceId id, int width, int height);


};