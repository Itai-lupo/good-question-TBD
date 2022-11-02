#pragma once
#include "core.hpp"
#include "windowRenderData.hpp"
#include "surface.hpp"

#include <thread>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <condition_variable>

#include <wayland-client.h>

class cpuRendering
{
    private:
        static void randname(char *buf);
        static int create_shm_file(void);
        static int allocate_shm_file(size_t size);
        static void allocateWindowCpuPool(surfaceId winId);
        
        static uint32_t *mapWindowCpuBuffer(surfaceId winId, uint32_t offset);


		static void wl_buffer_release(void *data, struct wl_buffer *wl_buffer);
        static constexpr struct wl_buffer_listener wl_buffer_listener = {
            .release = wl_buffer_release,
        };

        

        static void wlSurfaceFrameDone(void *data, wl_callback *cb, uint32_t time);
        static constexpr wl_callback_listener wlSurfaceFrameListener = 
        {
            .done = wlSurfaceFrameDone
        };


    public:
        static wl_buffer *allocateWindowBuffer(const surfaceId winId, uint32_t offset);
        static inline wl_shm *shm;

        struct renderInfo
        {
            surfaceId id;

            //to do move into a cpu rendering system
            int fd;
            wl_shm_pool *pool;
            uint32_t memoryPoolSize;
            uint8_t bufferInRender;
            uint8_t bufferToRender;
            uint8_t freeBuffer;
            uint32_t *buffer;
            int bufferSize;

            std::thread *renderThread;

            std::shared_ptr<std::shared_mutex> renderMutex{};
            std::shared_ptr<std::condition_variable_any> renderFinshed{};
            bool renderFinshedBool;

            renderInfo(): bufferInRender(0), bufferToRender(1), freeBuffer(2), 
                renderMutex(std::make_shared<std::shared_mutex>()), 
                renderFinshed(std::make_shared<std::condition_variable_any>())
            {}

        };

        struct idToRenderInfoEventIndexes
        {
            uint8_t gen = -1;
            uint8_t renderDataIndex = -1;
            uint8_t renderEventIndex = -1;
        };
        
        static inline std::vector<idToRenderInfoEventIndexes> idToIndex;  

        static inline std::vector<renderInfo> surfacesToRender;

        static inline std::vector< std::function<void(const windowRenderData&)>> renderEventListeners;
        static inline std::vector<surfaceId> renderEventId;

    
        static void renderWindow(surfaceId win); 

        static void reallocateWindowCpuPool(surfaceId winId);


        static void allocateSurfaceToRender(surfaceId winId);
        static void setRenderEventListeners(surfaceId winId, std::function<void(const windowRenderData&)> callback);
        
        static void deallocateSurfaceToRender(surfaceId winId);        
        static void unsetRenderEventListeners(surfaceId winId);

        static void resize(surfaceId id, int width, int height);


};