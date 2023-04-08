#pragma once 

#include "core.hpp"
#include "entityPool.hpp"
#include "log.hpp"

#include <array>
#include<vulkan/vulkan.hpp>

struct frameBufferInfo
{
    frameBufferInfo(framebufferId id, uint32_t width, uint32_t height): id(id), width(width), height(height){}
    
    uint32_t renderId;
    framebufferId id;

    textureId colorAttachmens[8];
    textureId depthAttachmen;
    uint32_t width, height;
    bool needToRebuild = true;
    vk::Framebuffer fbo;
};

class frameBuffersComponents
{
    private:
        entityPool *pool;

        std::vector<frameBufferInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            frameBuffersComponents *This = static_cast<frameBuffersComponents *>(data);
            This->deleteComponent(id);
        }

    public:
        frameBuffersComponents(entityPool *pool);
        ~frameBuffersComponents();

        void deleteComponent(entityId id);
        frameBufferInfo *getComponent(entityId id);
        void setComponent(entityId id, frameBufferInfo buffer);
        std::vector<frameBufferInfo>& getData()
        {
            return data;
        }
};
