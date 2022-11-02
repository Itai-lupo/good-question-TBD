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
            frameBufferInfo(framebufferId id, uint32_t width, uint32_t height): id(id), width(width), height(height){}
            
            uint32_t renderId;
            framebufferId id;

            textureId colorAttachmens[8]{[0 ... 7] = (uint32_t)-1, (uint32_t)-1};
            textureId depthAttachmen{(uint32_t)-1, (uint32_t)-1};
            uint32_t width, height;
            bool needToRebuild = true;
        };


        std::vector<frameBufferInfo> frameBuffers;

        struct idIndexes
        {
            uint32_t gen: 8 = -1;
            uint32_t index: 24;
        };        

        std::vector<idIndexes> idToIndex;  
        std::list<uint32_t> freeSlots;
        
        void rebuild(frameBufferInfo& fbo);
    public:
        frameBuffersManger(openglContext *context, textureManger *textures): context(context), textures(textures){}
        
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
