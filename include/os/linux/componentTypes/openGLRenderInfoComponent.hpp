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


struct openGlrenderInfo
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

   openGlrenderInfo():
        renderMutex(std::make_shared<std::shared_mutex>()), 
        renderFinshed(std::make_shared<std::condition_variable_any>())
    {}
};



class openGLRenderInfoComponent
{
    private:
        entityPool *pool;

        std::vector<openGlrenderInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            openGLRenderInfoComponent *This = static_cast<openGLRenderInfoComponent *>(data);
            This->deleteComponent(id);
        }

    public:
        openGLRenderInfoComponent(entityPool *pool);
        ~openGLRenderInfoComponent();

        void deleteComponent(entityId id);
       openGlrenderInfo *getComponent(entityId id);
        void setComponent(entityId id,openGlrenderInfo buffer);
        std::vector<openGlrenderInfo>& getData()
        {
            return data;
        }
};
