#pragma once
#include "core.hpp"

#include "entityPool.hpp"
#include "frameBuffersComponents.hpp"

#include <vulkan/vulkan.hpp>

#include <array>
#include <list>
#include <vector>
#include <queue>

namespace vulkanRenderEngine
{
    class framebuffers
    {
        private:
            static inline entityPool *framebuffersPool;
            static inline frameBuffersComponents *framebuffersData;
            static void rebuild(frameBufferInfo* fbo);
            static void destroyFrameBuffer(frameBufferInfo* fbo);
        public:
            
            static void init(entityPool *framebuffersPool);
            static void close();
            
            static void setFrameBufferData(frameBufferInfo info);
            static frameBufferInfo *getFrameBuffer(framebufferId FBOId);
            static void destroy(framebufferId FBOId);
            
            static framebufferId createFrameBuffer(renderPassId compatibleRenderPassId,  vk::ImageView *attachments, int width, int height);
            
            static void bind(framebufferId FBOId);
    };    
}

