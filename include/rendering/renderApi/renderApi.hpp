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
#include "uniformBufferComponents.hpp"

#include <queue>
#include <stdlib.h>
#include "vaoData.hpp"

enum class renderMode
{
    triangles,
    dots,
    lines
};

void defaultDeleteBuffer(void *buffer);

struct drawCallData
{
    shaderId shader;
    vaoId vertexArrayId;
    textureId texturesIds[32];
    uniformBufferId UBO;
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
    entityPool *commandBuffersPool;
    entityPool *renderPassesPool;
    entityPool *framebuffersPool;
    entityPool *texturesPool;
    entityPool *vaosPool;
    entityPool *shadersPool;
    entityPool *uniformBuffersPool;

    apiTypeComponents *commandBuffersApiType;
    apiTypeComponents *renderPassesApiType;
    apiTypeComponents *framebuffersApiType;
    apiTypeComponents *texturesApiType;
    apiTypeComponents *vaosApiType;
    apiTypeComponents *shadersApiType;
    apiTypeComponents *uniformBuffersApiType;

public:
    renderApi();
    ~renderApi();

    framebufferId allocFramebuffer(supportedRenderApis apiType);
    textureId allocTexture(supportedRenderApis apiType);
    vaoId allocVao(supportedRenderApis apiType);
    shaderId allocShader(supportedRenderApis apiType);
    uniformBufferId allocUniformBuffer(supportedRenderApis apiType);

    void deallocFramebuffer(framebufferId id);
    void deallocTexture(textureId id);
    void deallocVao(vaoId id);
    void deallocShader(shaderId id);
    void deallocUniformBuffer(uniformBufferId id);

    void setFramebuffer(frameBufferInfo data);
    void setTexture(textureInfo data);
    void setVao(vao data);
    void setShader(shaderInfo data);
    void setUniformBuffer(uniformBufferInfo data);

    frameBufferInfo *getFramebuffer(framebufferId id);
    textureInfo *getTexture(textureId id);
    vao *getVao(vaoId id);
    shaderInfo *getShader(shaderId id);
    uniformBufferInfo *getUniformBuffer(uniformBufferId id);

    void renderRequest(const renderRequestInfo &data);
};
