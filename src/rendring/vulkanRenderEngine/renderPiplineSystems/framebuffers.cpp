#include "vulkanRenderEngine/renderPiplineSystems/framebuffers.hpp"
#include "vulkanRenderEngine/renderPiplineSystems/renderPasses.hpp"
#include "device.hpp"

namespace vulkanRenderEngine
{
    void framebuffers::init(entityPool *framebuffersPool)
    {
        framebuffers::framebuffersPool = framebuffersPool;
        framebuffersData = new frameBuffersComponents(framebuffersPool);
    }

    void framebuffers::close()
    {
        for(int i = 0; i < framebuffersData->getData().size(); i++)
            destroyFrameBuffer(&framebuffersData->getData()[i]);

        delete framebuffersData;
    }

    


    void framebuffers::setFrameBufferData(frameBufferInfo info)
    {
        
    }

    frameBufferInfo *framebuffers::getFrameBuffer(framebufferId FBOId)
    {
        return framebuffersData->getComponent(FBOId);
    }


    framebufferId framebuffers::createFrameBuffer(renderPassId compatibleRenderPassId,  vk::ImageView *attachments, int width, int height)
    {
        framebufferId id = framebuffersPool->allocEntity();
        
        vk::RenderPass compatibleRenderPass = renderPasses::get(compatibleRenderPassId)->vkObject;

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.renderPass = compatibleRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;
        
        frameBufferInfo fbo(id, width, height);

        
        
        try
        {
            fbo.fbo =  device::getDevice().createFramebuffer(framebufferInfo); 
        }
        catch(vk::SystemError err)
        {
            LOG_FATAL("Failed to create framebuffer: " << err.what());
        }

        framebuffersData->setComponent(id, fbo);
        return id;
        
    }

    
    void framebuffers::bind(framebufferId FBOId)
    {

    }




    void framebuffers::rebuild(frameBufferInfo* fbo)
    {

    }

    void framebuffers::destroyFrameBuffer(frameBufferInfo* fbo)
    {
        device::getDevice().destroyFramebuffer(fbo->fbo);
    }


}

