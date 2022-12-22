#pragma once
#include "core.hpp"
#include "supportedRenderApis.hpp"
#include "gpuRenderData.hpp"


#include "entityPool.hpp"
#include "apiTypeComponents.hpp"

#include "frameBuffersComponents.hpp"
#include "shadersComponents.hpp"
#include "textureComponents.hpp"
#include "VAOsComponents.hpp"

#include <queue>
#include <stdlib.h>

enum class renderMode
{
    triangles,
    dots,
    lines
};


void defaultDeleteBuffer(void *buffer);


struct textureData
{
    textureId id;
    uint32_t width, height;
    
    struct bufferToUpload
    {
        uint32_t x, y, width, height, type; 
        textureFormat format; 
        void* pixels;
        void(*bufferDeleteCallback)(void*) = defaultDeleteBuffer;
    };
};


struct drawCallData
{
    shaderId shader;
    vaoId vertexArrayId;
    textureId texturesIds[32] ;
    renderMode mode;
};

struct renderRequestInfo
{
    framebufferId frameBufferId;

    std::vector<drawCallData> drawCalls;    
};

class renderApi
{
    private:
        entityPool *framebuffersPool;
        entityPool *texturesPool;
        entityPool *vaosPool;
        entityPool *shadersPool;
        
        apiTypeComponents *framebuffersApiType;
        apiTypeComponents *texturesApiType;
        apiTypeComponents *vaosApiType;
        apiTypeComponents *shadersApiType;
    public:
        renderApi();
        ~renderApi();

        framebufferId allocFramebuffer(supportedRenderApis apiType);
        textureId allocTexture(supportedRenderApis apiType);
        vaoId allocVao(supportedRenderApis apiType);
        shaderId allocShader(supportedRenderApis apiType);


        void deallocFramebuffer(framebufferId id);
        void deallocTexture(textureId id);
        void deallocVao(vaoId id);
        void deallocShader(shaderId id);

        void setFramebuffer(frameBufferInfo data);
        void setTexture(textureInfo data);
        void setVao(VAOInfo data);
        void setShader(shaderInfo data);


        frameBufferInfo *getFramebuffer(framebufferId id);
        textureInfo *getTexture(textureId id);
        VAOInfo *getVao(vaoId id);
        shaderInfo *getShader(shaderId id);

        void renderRequest(const renderRequestInfo& data);
        
};

