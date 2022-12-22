#include "frameBuffersManger.hpp"
#include "textureManger.hpp"

#include <Tracy.hpp>

namespace openGLRenderEngine
{
    void framebuffers::init(entityPool *framebuffers)
    {    framebuffersData = new frameBuffersComponents(framebuffers);
    }

    void framebuffers::close()
    {
        for (auto& FBO: framebuffersData->getData())
        {
            toDelete.push(FBO.renderId);
        }
        
        delete framebuffersData;
    }

    void framebuffers::setContext(openglContext *context)
    {
        framebuffers::context = context;
    }

    void framebuffers::handleRequsets()
    {
        if(toDelete.empty())
            return;

        uint32_t *FBOsToDelete = new uint32_t[toDelete.size()];
        uint32_t temp = toDelete.size();

        for (size_t i = 0; i < temp; i++)
        {
            FBOsToDelete[i] = toDelete.front();
            toDelete.pop();
        }
        
        GL_CALL(context, DeleteFramebuffers(temp, FBOsToDelete));  
    }

    void framebuffers::setFrameBufferData(frameBufferInfo info)
    {
        info.needToRebuild = true;
        framebuffersData->setComponent(info.id, info);
    }

    frameBufferInfo *framebuffers::getFrameBuffer(framebufferId id)
    {
        return framebuffersData->getComponent(id);
    }

    void framebuffers::bind(framebufferId id)
    {
        frameBufferInfo* temp = framebuffersData->getComponent(id);
        if(!temp)
            return;

        if(temp->needToRebuild)
            rebuild(temp);
        
        temp->needToRebuild = false;
        
        GL_CALL(context, BindFramebuffer(GL_FRAMEBUFFER, temp->renderId));  
        GL_CALL(context, Viewport(0, 0, temp->width, temp->height));
    }


    void framebuffers::rebuild(frameBufferInfo* fbo)
    {
        ZoneScopedN("frameBuffers rebuild");

        if(fbo->renderId)
            GL_CALL(context, DeleteFramebuffers(1, &fbo->renderId));
        

        GL_CALL(context, CreateFramebuffers(1, &fbo->renderId));

        for (size_t i = 0; i < 8; i++)
        {
            textureInfo *tempTex = textures::getTexture(fbo->colorAttachmens[i]);
            if(tempTex)
            {        
                tempTex->width = fbo->width;
                tempTex->height = fbo->height;
                tempTex->needToRebuild = true;
                textures::rebuild(fbo->colorAttachmens[i]);

                GL_CALL(context, 
                    NamedFramebufferTexture(
                    fbo->renderId, 
                    GL_COLOR_ATTACHMENT0 + i, 
                    tempTex->renderId, 0));
            }
        }    
        
        textureInfo *tempTex = textures::getTexture(fbo->depthAttachmen);
        if(tempTex)
        {
            tempTex->width = fbo->width;
            tempTex->height = fbo->height;
            tempTex->needToRebuild = true;
            textures::rebuild(fbo->depthAttachmen);

            GL_CALL(context, 
                NamedFramebufferTexture(
                fbo->renderId, 
                GL_DEPTH_STENCIL_ATTACHMENT, 
                tempTex->renderId, 0));
        }
            
        GL_CALL(context, BindFramebuffer(GL_FRAMEBUFFER, fbo->renderId));  
        
        auto fboStatus = context->openGLAPI->CheckFramebufferStatus(GL_FRAMEBUFFER);
        CONDTION_LOG_ERROR("Framebuffer is incomplete!: " << fboStatus,  fboStatus != GL_FRAMEBUFFER_COMPLETE)
    }
}
