#pragma once
#include "openglContext.hpp"
#include "textureManger.hpp"

#include "core.hpp"

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

        struct frameBufferInfo
        {
            uint32_t renderId;
            framebufferId id;

            textureId *colorAttachmens;
            textureId depthAttachmen;
            uint32_t width, hight;
            bool needToRebuild;
        };


        std::vector<frameBufferInfo> frameBuffers;

        struct idIndexes
        {
            uint32_t gen: 8;
            uint32_t index: 24;
        };        

        std::vector<idIndexes> idToIndex;  
        std::list<uint32_t> freeSlots;
        
        void rebuild(frameBufferInfo& fbo);
    public:
        frameBuffersManger(openglContext *context, textureManger *textures): context(context), textures(textures){}
        
        void handleRequsets();
        void bind(framebufferId FBOId);


        framebufferId createFrameBuffer(uint32_t width, uint32_t hight);

        void attachColorRenderTarget(framebufferId FBOId, textureId attachmentId, uint32_t attachmentSlot);
        void setDepthRenderTarget(framebufferId FBOId, textureId attachmentId);
        void resize(framebufferId FBOId, uint32_t width, uint32_t hight);

        void deleteFrameBuffer(framebufferId FBOId);
        
        uint32_t getWidth(framebufferId FBOId);
        uint32_t getHight(framebufferId FBOId);

        textureId *getColorAttachmens(framebufferId FBOId);
        textureId getDepthAttachmenId(framebufferId FBOId);
};
