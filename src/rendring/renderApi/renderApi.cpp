#include "renderApi.hpp"

#include "openGLRenderer.hpp"
#include "vulkanRenderEngine/renderer.hpp"

renderApi::renderApi()
{
    commandBuffersPool = new entityPool(255);
    framebuffersPool = new entityPool(255);
    renderPassesPool = new entityPool(255);
    texturesPool = new entityPool(1000);
    vaosPool = new entityPool(10000);
    shadersPool = new entityPool(1000);
    uniformBuffersPool = new entityPool(1000);

    commandBuffersApiType = new apiTypeComponents(commandBuffersPool);
    renderPassesApiType = new apiTypeComponents(renderPassesPool);
    framebuffersApiType = new apiTypeComponents(framebuffersPool);
    texturesApiType = new apiTypeComponents(texturesPool);
    vaosApiType = new apiTypeComponents(vaosPool);
    shadersApiType = new apiTypeComponents(shadersPool);
    uniformBuffersApiType = new apiTypeComponents(uniformBuffersPool);

    // openGLRenderEngine::framebuffers::init(framebuffersPool);
    // openGLRenderEngine::textures::init(texturesPool);
    // openGLRenderEngine::vaos::init(vaosPool);
    // openGLRenderEngine::shaders::init(shadersPool);
    // openGLRenderEngine::uniformBuffers::init(uniformBuffersPool);

    vulkanRenderEngine::commandBuffers::init(commandBuffersPool);
    vulkanRenderEngine::renderPasses::init(renderPassesPool);
    vulkanRenderEngine::framebuffers::init(framebuffersPool);
    vulkanRenderEngine::textures::init(texturesPool);
    vulkanRenderEngine::vaos::init(vaosPool);
    vulkanRenderEngine::graphicPiplines::init(shadersPool);
    vulkanRenderEngine::uniformBuffers::init(uniformBuffersPool);

    vulkanRenderEngine::renderer::init();
}

renderApi::~renderApi()
{
    // openGLRenderEngine::framebuffers::close();
    // openGLRenderEngine::textures::close();
    // openGLRenderEngine::vaos::close();
    // openGLRenderEngine::shaders::close();
    // openGLRenderEngine::uniformBuffers::close();

    vulkanRenderEngine::renderer::close();

    delete commandBuffersApiType;
    delete renderPassesApiType;
    delete framebuffersApiType;
    delete texturesApiType;
    delete vaosApiType;
    delete shadersApiType;
    delete uniformBuffersApiType;

    delete commandBuffersPool;
    delete renderPassesPool;
    delete framebuffersPool;
    delete texturesPool;
    delete vaosPool;
    delete shadersPool;
    delete uniformBuffersPool;
}

framebufferId renderApi::allocFramebuffer(supportedRenderApis apiType)
{
    framebufferId temp = framebuffersPool->allocEntity();
    framebuffersApiType->setComponent(temp, apiType);
    return temp;
}

textureId renderApi::allocTexture(supportedRenderApis apiType)
{
    textureId temp = texturesPool->allocEntity();
    texturesApiType->setComponent(temp, apiType);
    return temp;
}

vaoId renderApi::allocVao(supportedRenderApis apiType)
{
    vaoId temp = vaosPool->allocEntity();
    vaosApiType->setComponent(temp, apiType);
    return temp;
}

shaderId renderApi::allocShader(supportedRenderApis apiType)
{
    shaderId temp = shadersPool->allocEntity();
    shadersApiType->setComponent(temp, apiType);
    return temp;
}

uniformBufferId renderApi::allocUniformBuffer(supportedRenderApis apiType)
{
    uniformBufferId temp = uniformBuffersPool->allocEntity();
    uniformBuffersApiType->setComponent(temp, apiType);
    return temp;
}

void renderApi::deallocFramebuffer(framebufferId id)
{
    framebuffersPool->freeEntity(id);
}

void renderApi::deallocTexture(textureId id)
{
    texturesPool->freeEntity(id);
}

void renderApi::deallocVao(vaoId id)
{
    vaosPool->freeEntity(id);
}

void renderApi::deallocShader(shaderId id)
{
    shadersPool->freeEntity(id);
}

void renderApi::deallocUniformBuffer(uniformBufferId id)
{
    uniformBuffersPool->freeEntity(id);
}

void renderApi::setFramebuffer(frameBufferInfo data)
{
    switch (framebuffersApiType->getComponent(data.id))
    {
    case supportedRenderApis::openGl:
        openGLRenderEngine::framebuffers::setFrameBufferData(data);
        break;
    case supportedRenderApis::vulkan:
        vulkanRenderEngine::framebuffers::setFrameBufferData(data);
        break;

    default:
        break;
    }
}

void renderApi::setTexture(textureInfo data)
{
    switch (texturesApiType->getComponent(data.id))
    {
    case supportedRenderApis::openGl:
        openGLRenderEngine::textures::setTextureData(data);
        break;
    case supportedRenderApis::vulkan:
        vulkanRenderEngine::textures::setTextureData(data);
        break;

    default:
        break;
    }
}

void renderApi::setVao(vao data)
{
    switch (vaosApiType->getComponent(data.id))
    {
    case supportedRenderApis::openGl:
        // openGLRenderEngine::vaos::setVAOData(data);
        break;
    case supportedRenderApis::vulkan:
        vulkanRenderEngine::vaos::setVAOData(data);
        break;

    default:
        break;
    }
}

void renderApi::setShader(shaderInfo data)
{
    switch (shadersApiType->getComponent(data.id))
    {
    case supportedRenderApis::openGl:
        openGLRenderEngine::shaders::setShadersData(data);
        break;
    case supportedRenderApis::vulkan:
        // vulkanRenderEngine::graphicPiplines::set(data);
        break;

    default:
        break;
    }
}

void renderApi::setUniformBuffer(uniformBufferInfo data)
{
    switch (shadersApiType->getComponent(data.id))
    {
    case supportedRenderApis::openGl:
        openGLRenderEngine::uniformBuffers::setUniformBufferData(data);
        break;
    case supportedRenderApis::vulkan:
        vulkanRenderEngine::uniformBuffers::setUniformBufferData(data);
        break;

    default:
        break;
    }
}

frameBufferInfo *renderApi::getFramebuffer(framebufferId id)
{
    switch (shadersApiType->getComponent(id))
    {
    case supportedRenderApis::openGl:
        return openGLRenderEngine::framebuffers::getFrameBuffer(id);
        break;
    case supportedRenderApis::vulkan:
        return vulkanRenderEngine::framebuffers::getFrameBuffer(id);
        break;
    default:
        LOG_FATAL("id is not valid");
    }
    return nullptr;
}

textureInfo *renderApi::getTexture(textureId id)
{
    switch (shadersApiType->getComponent(id))
    {
    case supportedRenderApis::openGl:
        return openGLRenderEngine::textures::getTexture(id);
        break;
    case supportedRenderApis::vulkan:
        return vulkanRenderEngine::textures::getTexture(id);
        break;
    default:
        LOG_FATAL("id is not valid");
    }

    return nullptr;
}

vao *renderApi::getVao(vaoId id)
{
    switch (shadersApiType->getComponent(id))
    {
    case supportedRenderApis::openGl:
        return nullptr;
        // return openGLRenderEngine::vaos::getVAO(id);
        break;
    case supportedRenderApis::vulkan:
        return &vulkanRenderEngine::vaos::getVAO(id);
        break;
    default:
        LOG_FATAL("id is not valid");
    }

    return nullptr;
}

shaderInfo *renderApi::getShader(shaderId id)
{
    switch (shadersApiType->getComponent(id))
    {
    case supportedRenderApis::openGl:
        return openGLRenderEngine::shaders::getShaders(id);
        break;
    case supportedRenderApis::vulkan:
        // return vulkanRenderEngine::graphicPiplines::getShaders(id);
        break;
    default:
        LOG_FATAL("id is not valid");
    }

    return nullptr;
}

uniformBufferInfo *renderApi::getUniformBuffer(uniformBufferId id)
{
    switch (shadersApiType->getComponent(id))
    {
    case supportedRenderApis::openGl:
        return openGLRenderEngine::uniformBuffers::getUniformBuffer(id);
        break;
    case supportedRenderApis::vulkan:
        return vulkanRenderEngine::uniformBuffers::getUniformBuffer(id);
        break;
    default:
        LOG_FATAL("id is not valid");
    }

    return nullptr;
}

void renderApi::renderRequest(const renderRequestInfo &data)
{
    switch (framebuffersApiType->getComponent(data.frameBufferId))
    {
    case supportedRenderApis::openGl:
        openGLRenderEngine::openGLRenderer::renderRequest(data);
        break;
    case supportedRenderApis::vulkan:
        vulkanRenderEngine::renderer::renderRequest(data);
        break;
    default:
        LOG_FATAL("id is not valid");
    }
}

void defaultDeleteBuffer(void *buffer)
{
    free(buffer);
}