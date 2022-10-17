#include "frameBuffersManger.hpp"

void frameBuffersManger::rebuild(frameBufferInfo& fbo)
{
    if(fbo.renderId)
        GL_CALL(context, DeleteFramebuffers(1, &fbo.renderId));
    

    GL_CALL(context, CreateFramebuffers(1, &fbo.renderId));

    for (size_t i = 0; i < 8; i++)
    {
        // colorAttachmenData->setDimensions({ fbo.width, fbo.hight});
        if(fbo.colorAttachmens[i].gen != 255)
        {        
            textures->resize(fbo.colorAttachmens[i], fbo.width, fbo.hight);
            textures->rebuild(fbo.colorAttachmens[i]);
            GL_CALL(context, 
                NamedFramebufferTexture(
                fbo.renderId, 
                GL_COLOR_ATTACHMENT0 + i, 
                textures->getRenderId(fbo.colorAttachmens[i]), 0));

            // GLuint framebufferTex;
            // GL_CALL(context, CreateTextures(GL_TEXTURE_2D, 1, &framebufferTex));
            // GL_CALL(context, TextureParameteri(framebufferTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            // GL_CALL(context, TextureParameteri(framebufferTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            // GL_CALL(context, TextureParameteri(framebufferTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            // GL_CALL(context, TextureParameteri(framebufferTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            // GL_CALL(context, TextureStorage2D(framebufferTex, 1, GL_RGBA8, fbo.width, fbo.hight));
            // GL_CALL(context, NamedFramebufferTexture(fbo.renderId, GL_COLOR_ATTACHMENT0, framebufferTex, 0));
            // textures->setRenderId(fbo.colorAttachmens[i], framebufferTex);

        }
    }
    

    if(fbo.depthAttachmen.index != -1)
    {
        // parentContainer->getDepthAttachmen()->setDimensions({ parentContainer->getWidth(), parentContainer->getHight()});
        
        // GL_CALL(context, 
        //     NamedFramebufferTexture(
        //     fbo.renderId, 
        //     GL_DEPTH_STENCIL_ATTACHMENT, 
        //     fbo.depthAttachmen.index, 0));
    }
        
    GL_CALL(context, BindFramebuffer(GL_FRAMEBUFFER, fbo.renderId));  
    
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
    if(idToIndex[FBOId.index].gen != FBOId.gen)
        return;

    frameBufferInfo& temp = frameBuffers[idToIndex[FBOId.index].index];
    if(temp.needToRebuild)
        rebuild(temp);
    
    temp.needToRebuild = false;
    GL_CALL(context, BindFramebuffer(GL_FRAMEBUFFER, temp.renderId));  
    GL_CALL(context, Viewport(0, 0, temp.width, temp.hight));

}

framebufferId frameBuffersManger::createFrameBuffer(uint32_t width, uint32_t hight)
{
    framebufferId id;
    if(!freeSlots.empty())
    {
        id = {
            .gen = idToIndex[freeSlots.front()].gen,
            .index = freeSlots.front()
        };
        freeSlots.pop_front();
    }
    else
    {
        id = {
            .gen = 0,
            .index = (uint32_t)idToIndex.size()
        };
        idToIndex.push_back({0, (uint32_t)frameBuffers.size()});
    }

    frameBufferInfo info;
    info.id = id;
    info.needToRebuild = true;
    info.colorAttachmens = new textureId[8]{[0 ... 7] = (uint32_t)-1, (uint32_t)-1};
    info.depthAttachmen = {(uint32_t)-1, (uint32_t)-1};

    info.width = width;
    info.hight = hight;
    
    idToIndex[id.index].index = frameBuffers.size();
    frameBuffers.push_back(info);
    return id;
}

void frameBuffersManger::attachColorRenderTarget(framebufferId FBOId, textureId attachmentId, uint32_t attachmentSlot)
{
    if(idToIndex[FBOId.index].gen != FBOId.gen)
        return;

    frameBufferInfo& temp = frameBuffers[idToIndex[FBOId.index].index];
    temp.colorAttachmens[attachmentSlot] = attachmentId;
    temp.needToRebuild = true;

}

void frameBuffersManger::setDepthRenderTarget(framebufferId FBOId, textureId attachmentId)
{
    if(idToIndex[FBOId.index].gen != FBOId.gen)
        return;

    frameBufferInfo& temp = frameBuffers[idToIndex[FBOId.index].index];
    temp.needToRebuild = true;

}

void frameBuffersManger::resize(framebufferId FBOId, uint32_t width, uint32_t hight)
{
    if(idToIndex[FBOId.index].gen != FBOId.gen)
        return;

    frameBufferInfo& temp = frameBuffers[idToIndex[FBOId.index].index];
    temp.needToRebuild = true;

}

void frameBuffersManger::deleteFrameBuffer(framebufferId FBOId)
{
    if(FBOId.gen != idToIndex[FBOId.index].gen)
        return;

    frameBufferInfo temp = frameBuffers[idToIndex[FBOId.index].index];
    
    toDelete.push(temp.renderId);
    delete[] temp.colorAttachmens;

    idToIndex[FBOId.index].gen++;
    
    frameBufferInfo last = frameBuffers[frameBuffers.size() - 1];
    frameBuffers[idToIndex[FBOId.index].index] = last;
    idToIndex[last.id.index].index = idToIndex[FBOId.index].index;

    frameBuffers.pop_back();
    idToIndex[FBOId.index].index = -1;
    freeSlots.push_back(FBOId.index);
}

uint32_t frameBuffersManger::getWidth(framebufferId FBOId)
{

    if(idToIndex[FBOId.index].gen != FBOId.gen)
        return - 1;

    frameBufferInfo& temp = frameBuffers[idToIndex[FBOId.index].index];
    return temp.width;
}

uint32_t frameBuffersManger::getHight(framebufferId FBOId)
{
    if(idToIndex[FBOId.index].gen != FBOId.gen)
        return -1;

    frameBufferInfo& temp = frameBuffers[idToIndex[FBOId.index].index];
    return temp.hight;
}

textureId *frameBuffersManger::getColorAttachmens(framebufferId FBOId)
{
    if(idToIndex[FBOId.index].gen != FBOId.gen)
        return NULL;

    frameBufferInfo& temp = frameBuffers[idToIndex[FBOId.index].index];
    return temp.colorAttachmens;
}

textureId frameBuffersManger::getDepthAttachmenId(framebufferId FBOId)
{
    if(idToIndex[FBOId.index].gen != FBOId.gen)
        return {(uint32_t)-1, (uint32_t)-1};

    frameBufferInfo& temp = frameBuffers[idToIndex[FBOId.index].index];
    return temp.depthAttachmen;
}

