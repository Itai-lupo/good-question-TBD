#pragma once 
#include "core.hpp"
#include "entityPool.hpp"
#include "log.hpp"
#include "gpuRenderData.hpp"

#include <array>
#include <thread>
#include <memory>
#include <shared_mutex>
#include <condition_variable>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <glad/gl.h>
#include <EGL/egl.h>


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
    
    // unsigned int textureBufferId = 0;

    wl_egl_window *eglWindow = NULL;
    EGLSurface eglSurface = NULL;

    EGLImage eglImage;

    void(*renderFuncion)(const gpuRenderData&);

    renderInfo():
        renderMutex(std::make_shared<std::shared_mutex>()), 
        renderFinshed(std::make_shared<std::condition_variable_any>())
    {}
};



class gpuRenderInfoComponent
{
    private:
        entityPool *pool;

        std::vector<renderInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            gpuRenderInfoComponent *This = static_cast<gpuRenderInfoComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        gpuRenderInfoComponent(entityPool *pool);
        ~gpuRenderInfoComponent();

        void deleteComponent(entityId id);
        renderInfo *getComponent(entityId id);
        void setComponent(entityId id, renderInfo buffer);
        std::vector<renderInfo>& getData()
        {
            return data;
        }
};
