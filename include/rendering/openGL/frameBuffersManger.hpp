#pragma once
#include "openglContext.hpp"
#include "textureManger.hpp"

#include "core.hpp"

#include "entityPool.hpp"
#include "frameBuffersCompontents.hpp"

#include <array>
#include <list>
#include <vector>
#include <queue>


class frameBuffersManger
{
    private:
        openglContext *context;
        textureManger *textures;

        std::queue<uint32_t> toDelete;


        entityPool framebuffers;
        frameBuffersCompontents *framebuffersData;
        void rebuild(frameBufferInfo* fbo);
    public:
        frameBuffersManger(openglContext *context, textureManger *textures): 
            context(context), textures(textures), framebuffers(500), framebuffersData(new frameBuffersCompontents(&framebuffers))
            {}
        
        void handleRequsets();
        void bind(framebufferId FBOId);


        framebufferId createFrameBuffer(uint32_t width, uint32_t height);

        void attachColorRenderTarget(framebufferId FBOId, textureId attachmentId, uint32_t attachmentSlot);
        void setDepthRenderTarget(framebufferId FBOId, textureId attachmentId);
        void resize(framebufferId FBOId, uint32_t width, uint32_t height);

        void deleteFrameBuffer(framebufferId FBOId);
        
        uint32_t getWidth(framebufferId FBOId);
        uint32_t getheight(framebufferId FBOId);

        textureId *getColorAttachmens(framebufferId FBOId);
        textureId getDepthAttachmenId(framebufferId FBOId);
};
