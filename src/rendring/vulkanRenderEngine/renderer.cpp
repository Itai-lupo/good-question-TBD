#include "renderer.hpp"
#include "instance.hpp"
#include "debug.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "queueFamilys.hpp"
#include "memoryManger.hpp"

#include <Tracy.hpp>
#include <sys/prctl.h>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
};

const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
};

void defaultRenderCallback(vkSurfaceId surfaceId, commandBufferId cmdId, uint32_t imageIndex)
{
    commandBufferInfo cmdBufferData = *vulkanRenderEngine::commandBuffers::getBuffer(cmdId);
#define swapchainData vulkanRenderEngine::swapchain::getSwapChain(surfaceId)

    try
    {
        vk::CommandBufferBeginInfo beginInfo;
        cmdBufferData.vkObject.begin(beginInfo);
        vk::RenderPassBeginInfo renderPassCreateInfo{};

        renderPassCreateInfo.renderPass = vulkanRenderEngine::renderPasses::get(swapchainData->renderPass)->vkObject;

        if (vulkanRenderEngine::framebuffers::getFrameBuffer(swapchainData->swapChainFramebuffers[imageIndex]))
            renderPassCreateInfo.framebuffer = vulkanRenderEngine::framebuffers::getFrameBuffer(swapchainData->swapChainFramebuffers[imageIndex])->fbo;
        else
            return;
        renderPassCreateInfo.renderArea.offset = vk::Offset2D{0, 0};
        renderPassCreateInfo.renderArea.extent = swapchainData->extent;

        vk::ClearValue clearColor{std::array<float, 4>{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassCreateInfo.clearValueCount = 1;
        renderPassCreateInfo.pClearValues = &clearColor;

        cmdBufferData.vkObject.beginRenderPass(renderPassCreateInfo, vk::SubpassContents::eInline);

        cmdBufferData.vkObject.bindPipeline(vk::PipelineBindPoint::eGraphics, vulkanRenderEngine::graphicPiplines::get(swapchainData->graphicPiplineId)->graphicsPipeline);

        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchainData->extent.width);
        viewport.height = static_cast<float>(swapchainData->extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        cmdBufferData.vkObject.setViewport(0, 1, &viewport);

        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D{0, 0};
        scissor.extent = swapchainData->extent;
        cmdBufferData.vkObject.setScissor(0, 1, &scissor);
        static vkMemoryId bufferId = {(uint32_t)-1, (uint8_t)1};
        if (bufferId.index == (uint32_t)-1)
        {
            vulkanRenderEngine::memoryBuffer info;
            info.bufferSize = sizeof(vertices[0]) * vertices.size();
            info.usage = vk::BufferUsageFlagBits::eVertexBuffer;
            info.sharingMode = vk::SharingMode::eExclusive;
            bufferId = vulkanRenderEngine::memoryManger::mapBuffer(info);
            vulkanRenderEngine::memoryManger::writeToBuffer(bufferId, (void *)vertices.data());
        }

        vk::Buffer vertexBuffers[] = {vulkanRenderEngine::memoryManger::getBuffer(bufferId).vkBuffer};
        vk::DeviceSize offsets[] = {0};

        cmdBufferData.vkObject.bindVertexBuffers(0, 1, vertexBuffers, offsets);
        cmdBufferData.vkObject.draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);

        cmdBufferData.vkObject.draw(3, 1, 0, 0);
        cmdBufferData.vkObject.endRenderPass();
        cmdBufferData.vkObject.end();
    }
    catch (vk::SystemError err)
    {
        LOG_FATAL("failed to begin recording command buffer! " << err.what());
    }
#undef swapchainData
}

namespace vulkanRenderEngine
{
    void renderer::init()
    {
        vulkanWindosPool = new entityPool(256);
        surfaces = new vkSurfaceComponents(vulkanWindosPool);
        instance::create();
        debug::setup();
        swapchain::init(vulkanWindosPool);
        memoryManger::init();
    }

    void renderer::close()
    {
        if (deviceWasCreated)
        {
            isAlive = false;
            drawThread->join();
            device::getDevice().waitIdle();
            commandBuffers::close();
            textures::close();
            vaos::close();
            graphicPiplines::close();
            uniformBuffers::close();
            renderPasses::close();
            swapchain::close();
            framebuffers::close();
            memoryManger::close();
            device::destroy();
        }

        for (auto &s : surfaces->getData())
            instance::getInstance().destroySurfaceKHR(s);
        debug::close();
        instance::destroy();

        delete surfaces;
        delete vulkanWindosPool;
    }

    vkSurfaceId renderer::createSurface(vk::SurfaceKHR surfaceToCreate, int width, int height)
    {
        if (!deviceWasCreated)
        {
            initDevice(surfaceToCreate);
        }

        vkSurfaceId id = vulkanWindosPool->allocEntity();

        surfaces->setComponent(id, surfaceToCreate);
        swapchain::create(id, surfaceToCreate, width, height);

        renderPassInfo renderPassData;
        renderPassData.format = swapchain::getSwapChain(id)->formatToUse.format;
        renderPassId renderPass = renderPasses::create(renderPassData);

        graphicsPipelineInfo graphicsPipelineData;
        graphicsPipelineData.fragShaderCodePath = "assets/shaders/fragment/vulkanPipline.frag.spv";
        graphicsPipelineData.vertShaderCodePath = "assets/shaders/vertex/vulkanPipline.vert.spv";
        graphicsPipelineData.renderPassId = renderPass;

        vao vaoInfo;
        vaoInfo.attributes->push_back({0, 0, offsetof(Vertex, pos), vk::Format::eR32G32Sfloat});
        vaoInfo.attributes->push_back({0, 1, offsetof(Vertex, color), vk::Format::eR32G32B32Sfloat});
        vaoInfo.bindings->push_back({sizeof(Vertex), false});
        graphicsPipelineData.vao = vaos::createVAO(vaoInfo);

        shaderId graphicPiplineId = graphicPiplines::create(graphicsPipelineData);

        for (int i = 0; i < swapchain::getSwapChain(id)->swapChainImagesViews.size(); i++)
        {
            swapchain::getSwapChain(id)->swapChainFramebuffers.push_back(
                framebuffers::createFrameBuffer(
                    renderPass,
                    &swapchain::getSwapChain(id)->swapChainImagesViews[i],
                    width,
                    height));
        }

        vk::SemaphoreCreateInfo semaphoreInfo{};
        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
        try
        {
            swapchain::getSwapChain(id)->imageAvailableSemaphore = device::getDevice().createSemaphore(semaphoreInfo);
            swapchain::getSwapChain(id)->renderFinishedSemaphore = device::getDevice().createSemaphore(semaphoreInfo);
            swapchain::getSwapChain(id)->inFlightFence = device::getDevice().createFence(fenceInfo);
        }
        catch (vk::SystemError err)
        {
            LOG_ERROR("failed to create semaphores!");
        }

        commandBufferInfo cmdBufferData;

        cmdBufferData.level = vk::CommandBufferLevel::ePrimary;
        cmdBufferData.poolId = poolId;
        swapchain::getSwapChain(id)->renderPass = renderPass;
        swapchain::getSwapChain(id)->graphicPiplineId = graphicPiplineId;
        swapchain::getSwapChain(id)->complete = true;

        drawRequsts.enqueue({id, commandBuffers::createBuffer(cmdBufferData), defaultRenderCallback});
        return id;
    }

    void renderer::destroySurface()
    {
    }

    void renderer::renderRequest(const renderRequestInfo &dataToRender)
    {
    }

    bool renderer::isSurfaceValid(vkSurfaceId id)
    {
        return vulkanWindosPool->isIdValid(id);
    }

    void renderer::drawLoop()
    {
        std::string thradNameA = "vulkan rendering";
        tracy::SetThreadName(thradNameA.c_str());
        prctl(PR_SET_NAME, thradNameA.c_str());
        isAlive = true;

        while (isAlive)
        {
            ZoneScoped;
            handleAllTranferRequestes();
            handleRenderRequest();
            handlePresentRequset();
        }
    }

    void renderer::handleAllTranferRequestes()
    {
        ZoneScoped;
    }

    void renderer::handleRenderRequest()
    {
        ZoneScoped;
        drawRequst temp = drawRequsts.dequeue();
        swapChainInfo *swapchainData = swapchain::getSwapChain(temp.surfaceId);
        uint64_t tempPtr = (uint64_t)swapchainData;
        device::getDevice().waitForFences(1, &swapchainData->inFlightFence, VK_TRUE, UINT64_MAX);
        swapchainData = swapchain::getSwapChain(temp.surfaceId);

        auto [res, imageIndex] = device::getDevice().acquireNextImageKHR(swapchainData->swapChain, UINT64_MAX, swapchainData->imageAvailableSemaphore, VK_NULL_HANDLE);
        if (res == vk::Result::eErrorOutOfDateKHR)
        {
            recreateSwapChain(swapchainData->id);
        }
        else if (res != vk::Result::eSuccess && res != vk::Result::eSuboptimalKHR)
        {
            LOG_FATAL("failed to submit draw command buffer!");
        }

        device::getDevice().resetFences(1, &swapchainData->inFlightFence);
        temp.callback(temp.surfaceId, temp.cmdId, imageIndex);

        commandBufferInfo cmdBufferData = *commandBuffers::getBuffer(temp.cmdId);
        vk::Semaphore waitSemaphores[] = {swapchainData->imageAvailableSemaphore};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBufferData.vkObject;

        vk::Semaphore signalSemaphores[] = {swapchainData->renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        try
        {
            ZoneScoped;

            vk::Result r = device::getGraphicsQueue().submit(1, &submitInfo, swapchainData->inFlightFence);
        }
        catch (vk::SystemError err)
        {
            LOG_FATAL("failed to submit draw command buffer!" << err.what());
        }
        presentRequsts.enqueue({temp.surfaceId, temp.cmdId, temp.callback, submitInfo, imageIndex});
    }

    void renderer::handlePresentRequset()
    {
        ZoneScoped;
        auto [id, cmdId, callback, submitInfo, imageIndex] = presentRequsts.dequeue();
        swapChainInfo *swapchainData = swapchain::getSwapChain(id);
        vk::Semaphore signalSemaphores[] = {swapchainData->renderFinishedSemaphore};

        vk::PresentInfoKHR presentInfo{};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        vk::SwapchainKHR swapChain[] = {swapchainData->swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChain;
        presentInfo.pImageIndices = &imageIndex;
        {
            ZoneScoped;
            vk::Result r = device::getPresentQueue().presentKHR(&presentInfo);
            if (r == vk::Result::eErrorOutOfDateKHR || r == vk::Result::eSuboptimalKHR || swapchainData->outOfData)
            {
                recreateSwapChain(id);
            }
            else if (r != vk::Result::eSuccess)
            {
                LOG_FATAL("failed to submit draw command buffer!");
            }
        }

        drawRequsts.enqueue({id, cmdId, callback});
    }

    void renderer::resize(vkSurfaceId id, int width, int height)
    {
        if (swapchain::swapchainsInfo->getComponent(id))
        {
            swapchain::swapchainsInfo->getComponent(id)->outOfData = true;
            swapchain::swapchainsInfo->getComponent(id)->height = height;
            swapchain::swapchainsInfo->getComponent(id)->width = width;
        }
    }

    void renderer::recreateSwapChain(vkSurfaceId id)
    {
        device::getDevice().waitIdle();
        swapChainInfo temp = *swapchain::getSwapChain(id);

        swapchain::resize(id, surfaces->getComponent(id), temp.width, temp.height);
        for (int i = 0; i < swapchain::getSwapChain(id)->swapChainImagesViews.size(); i++)
        {
            swapchain::getSwapChain(id)->swapChainFramebuffers.push_back(
                framebuffers::createFrameBuffer(
                    temp.renderPass,
                    &swapchain::getSwapChain(id)->swapChainImagesViews[i],
                    temp.width,
                    temp.height));
        }

        swapchain::getSwapChain(id)->imageAvailableSemaphore = temp.imageAvailableSemaphore;
        swapchain::getSwapChain(id)->renderFinishedSemaphore = temp.renderFinishedSemaphore;
        swapchain::getSwapChain(id)->inFlightFence = temp.inFlightFence;

        swapchain::getSwapChain(id)->renderPass = temp.renderPass;
        swapchain::getSwapChain(id)->graphicPiplineId = temp.graphicPiplineId;
        swapchain::getSwapChain(id)->cmdId = temp.cmdId;
        swapchain::getSwapChain(id)->outOfData = false;
        swapchain::getSwapChain(id)->complete = true;
    }

    void renderer::initDevice(vk::SurfaceKHR surfaceToCreate)
    {
        device::create(surfaceToCreate);
        LOG_INFO("vulkan renderer init successfully");

        commandPoolInfo commandPoolData;
        commandPoolData.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolData.queueFamilyIndex = queueFamilys::getGraphicsFamilyQueue().value();
        poolId = commandBuffers::createPool(commandPoolData);

        drawThread = new std::thread(drawLoop);
        deviceWasCreated = true;
    }
}