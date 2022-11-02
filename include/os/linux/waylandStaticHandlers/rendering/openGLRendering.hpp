#pragma once
#include "core.hpp"
#include "windowRenderData.hpp"
#include "surface.hpp"
#include "openglContext.hpp"
#include "openGLRenderer.hpp"


#include <thread>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <condition_variable>

#include <wayland-client.h>
#include <wayland-egl.h>


class Shader;

class openGLRendering
{
    private:

        static void wlSurfaceFrameDone(void *data, wl_callback *cb, uint32_t time);
        static constexpr wl_callback_listener wlSurfaceFrameListener = 
        {
            .done = wlSurfaceFrameDone
        };


    public:
        static inline bool wasInit = false;
        static inline Shader *textureShader;
        static inline openglContext *context;
        static inline openGLRenderer *renderer;

        struct renderInfo
        {
            surfaceId id;

            textureId bufferInRenderTex;
            textureId bufferToRenderTex;
            textureId freeBufferTex;
            
            framebufferId freeBuffer;
            framebufferId bufferToRender;
            framebufferId bufferInRender;
            
            int bufferSize;

            std::thread *renderThread;

            std::shared_ptr<std::shared_mutex> renderMutex{};
            std::shared_ptr<std::condition_variable_any> renderFinshed{};
            bool renderFinshedBool;
            
            unsigned int textureBufferId = 0;

            renderInfo():
                renderMutex(std::make_shared<std::shared_mutex>()), 
                renderFinshed(std::make_shared<std::condition_variable_any>())
            {}

            wl_egl_window *eglWindow = NULL;
            EGLSurface eglSurface = NULL;

            EGLImage eglImage;
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

        static void init();

        static void allocateSurfaceToRender(surfaceId winId);
        static void setRenderEventListeners(surfaceId winId, std::function<void(const windowRenderData&)> callback);
        
        static void deallocateSurfaceToRender(surfaceId winId);        
        static void unsetRenderEventListeners(surfaceId winId);

        static void resize(surfaceId id, int width, int height);

};