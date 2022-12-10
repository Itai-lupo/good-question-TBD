#include "frameBuffersManger.hpp"
#include <Tracy.hpp>

void frameBuffersManger::rebuild(frameBufferInfo* fbo)
{
    ZoneScopedN("frameBuffers rebuild");

    if(fbo->renderId)
        GL_CALL(context, DeleteFramebuffers(1, &fbo->renderId));
    

    GL_CALL(context, CreateFramebuffers(1, &fbo->renderId));

    for (size_t i = 0; i < 8; i++)
    {
        if(fbo->colorAttachmens[i].gen != 255)
        {        
            textures->resize(fbo->colorAttachmens[i], fbo->width, fbo->height);
            textures->rebuild(fbo->colorAttachmens[i]);
            GL_CALL(context, 
                NamedFramebufferTexture(
                fbo->renderId, 
                GL_COLOR_ATTACHMENT0 + i, 
                textures->getRenderId(fbo->colorAttachmens[i]), 0));
        }
    }
    

    if(fbo->depthAttachmen.index != -1)
    {
        // parentContainer->getDepthAttachmen()->setDimensions({ parentContainer->getWidth(), parentContainer->getheight()});
        
        // GL_CALL(context, 
        //     NamedFramebufferTexture(
        //     fbo->renderId, 
        //     GL_DEPTH_STENCIL_ATTACHMENT, 
        //     fbo->depthAttachmen.index, 0));
    }
        
    GL_CALL(context, BindFramebuffer(GL_FRAMEBUFFER, fbo->renderId));  
    
	auto fboStatus = context->openGLAPI->CheckFramebufferStatus(GL_FRAMEBUFFER);
    CONDTION_LOG_ERROR("Framebuffer is incomplete!: " << fboStatus,  fboStatus != GL_FRAMEBUFFER_COMPLETE)

}

void frameBuffersManger::handleRequsets()
{
    while (!toDelete.empty())
    {
        GL_CALL(context, DeleteFramebuffers(1, &toDelete.front()));  
        toDelete.pop();
    }   
}


void frameBuffersManger::bind(framebufferId FBOId)
{
    frameBufferInfo* temp = framebuffersData->getComponent(FBOId);
    if(!temp)
        return;

    if(temp->needToRebuild)
        rebuild(temp);
    
    temp->needToRebuild = false;
    GL_CALL(context, BindFramebuffer(GL_FRAMEBUFFER, temp->renderId));  
    GL_CALL(context, Viewport(0, 0, temp->width, temp->height));
}

framebufferId frameBuffersManger::createFrameBuffer(uint32_t width, uint32_t height)
{
    framebufferId id = framebuffers.allocEntity();

    frameBufferInfo info(id, width, height);
    framebuffersData->setComponent(id, info);
    return id;
}

void frameBuffersManger::attachColorRenderTarget(framebufferId FBOId, textureId attachmentId, uint32_t attachmentSlot)
{
    frameBufferInfo* temp = framebuffersData->getComponent(FBOId);
    if(!temp)
        return;

    temp->colorAttachmens[attachmentSlot] = attachmentId;
    temp->needToRebuild = true;
}

void frameBuffersManger::setDepthRenderTarget(framebufferId FBOId, textureId attachmentId)
{
    frameBufferInfo* temp = framebuffersData->getComponent(FBOId);
    if(!temp)
        return;

    temp->depthAttachmen = attachmentId;
    temp->needToRebuild = true;
}

void frameBuffersManger::resize(framebufferId FBOId, uint32_t width, uint32_t height)
{
    frameBufferInfo* temp = framebuffersData->getComponent(FBOId);
    if(!temp)
        return;

    temp->width = width;
    temp->height = height;
    temp->needToRebuild = true;

}

void frameBuffersManger::deleteFrameBuffer(framebufferId FBOId)
{
    frameBufferInfo* temp = framebuffersData->getComponent(FBOId);
    if(!temp)
        return;
        
    toDelete.push(temp->renderId);
    framebuffers.freeEntity(FBOId);
}

uint32_t frameBuffersManger::getWidth(framebufferId FBOId)
{

    frameBufferInfo* temp = framebuffersData->getComponent(FBOId);
    if(!temp)
        return -1;

    return temp->width;
}

uint32_t frameBuffersManger::getheight(framebufferId FBOId)
{
    
    frameBufferInfo* temp = framebuffersData->getComponent(FBOId);
    if(!temp)
        return -1;

    return temp->height;
}

textureId *frameBuffersManger::getColorAttachmens(framebufferId FBOId)
{
    frameBufferInfo* temp = framebuffersData->getComponent(FBOId);
    if(!temp)
        return nullptr;

    return temp->colorAttachmens;
}

textureId frameBuffersManger::getDepthAttachmenId(framebufferId FBOId)
{
    frameBufferInfo* temp = framebuffersData->getComponent(FBOId);
    if(!temp)
        return entityId();

    return temp->depthAttachmen;
}

