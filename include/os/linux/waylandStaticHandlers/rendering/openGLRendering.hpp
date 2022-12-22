#pragma once
#include "core.hpp"
#include "gpuRenderData.hpp"
#include "surface.hpp"
#include "openglContext.hpp"
#include "openGLRenderer.hpp"
#include "entityPool.hpp"
#include "gpuRenderInfoComponent.hpp"
#include "renderApi.hpp"

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

        static void renderWindow(surfaceId id);
        static inline gpuRenderInfoComponent *renderData;
        static inline renderApi *api;
    public:
        static inline bool wasInit = false;
        static inline Shader *textureShader;
        static inline openglContext *context;

        static void init(entityPool *surfacesPool, renderApi *api);
        static void close();

        static void allocateSurfaceToRender(surfaceId winId, void(*callback)(const gpuRenderData&));
        static void setRenderEventListeners(surfaceId winId, void(*callback)(const gpuRenderData&));
        
        static void deallocateSurfaceToRender(surfaceId winId);        
        
        static void resize(surfaceId id, int width, int height);


};