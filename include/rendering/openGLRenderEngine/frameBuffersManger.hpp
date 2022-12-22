#pragma once
#include "openglContext.hpp"

#include "core.hpp"

#include "entityPool.hpp"
#include "frameBuffersComponents.hpp"

#include <array>
#include <list>
#include <vector>
#include <queue>

namespace openGLRenderEngine
{
    class framebuffers
    {
        private:
            static inline openglContext *context;

            static inline frameBuffersComponents *framebuffersData;
            static void rebuild(frameBufferInfo* fbo);
        public:
            static inline std::queue<uint32_t> toDelete;
            
            static void init(entityPool *framebuffers);
            static void close();
            
            static void setContext(openglContext *context);
            static void handleRequsets();

            static void setFrameBufferData(frameBufferInfo info);
            static frameBufferInfo *getFrameBuffer(framebufferId FBOId);
            
            static void bind(framebufferId FBOId);
    };    
}

