#include "renderer.hpp"
#include "instance.hpp"
#include "debug.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "graphicPiplines.hpp"
#include "framebuffers.hpp"
#include "commandBuffers.hpp"

namespace vulkanRenderEngine
{
    void renderer::init()
    {
        vulkanWindosPool = new entityPool(256);
        surfaces = new vkSurfaceComponents(vulkanWindosPool);
        instance::create();
        debug::setup();
        swapchain::init(vulkanWindosPool);
        graphicPiplines::init(vulkanWindosPool);
    }

    void renderer::close()
    {
        if(deviceWasCreated)
        {
            framebuffers::close();
            graphicPiplines::close();
            swapchain::close();
            device::destroy();
        }

        for(auto& s: surfaces->getData())
            instance::getInstance().destroySurfaceKHR(s);
        debug::close();
        instance::destroy();

        delete surfaces;
        delete vulkanWindosPool;
    }

    vkSurfaceId renderer::createSurface(vk::SurfaceKHR surfaceToCreate, int width, int height)
    {
        if(!deviceWasCreated)
        {
            device::create(surfaceToCreate);
            commandBuffers::init(new entityPool(200));
            deviceWasCreated = true;
        }

        vkSurfaceId id = vulkanWindosPool->allocEntity();
        surfaces->setComponent(id, surfaceToCreate);
        swapchain::create(id, surfaceToCreate, width, height);


        //need to move to the shaders manger and rename it to graphics pipline manger + extend it's api to acommdate that
        renderPassInfo renderPassData;
        renderPassData.format = swapchain::getSwapChain(id)->formatToUse.format;
        renderPassId renderPass = renderPasses::create(renderPassData);

        graphicsPipelineInfo graphicsPipelineData;
        graphicsPipelineData.fragShaderCodePath = "assets/shaders/fragment/vulkanPipline.frag.spv";
        graphicsPipelineData.vertShaderCodePath = "assets/shaders/vertex/vulkanPipline.vert.spv";
        graphicsPipelineData.renderPassId = renderPass;
        shaderId graphicPiplineId =  graphicPiplines::create(graphicsPipelineData);
        
        commandBufferInfo cmdBufferData;
        commandBufferId cmdId = commandBuffers::create(cmdBufferData);

        for(int i = 0; i < swapchain::getSwapChain(id)->swapChainImagesViews.size(); i++)
        {
            swapchain::getSwapChain(id)->swapChainFramebuffers.push_back(
            framebuffers::createFrameBuffer(
                renderPass, 
                &swapchain::getSwapChain(id)->swapChainImagesViews[i],
                width,
                height));
        }

        LOG_INFO("vulkan renderer init successfully")
        vk::Semaphore imageAvailableSemaphore;
        vk::Semaphore renderFinishedSemaphore;
        vk::Fence inFlightFence;

        vk::SemaphoreCreateInfo semaphoreInfo{};

        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        try
        {
            imageAvailableSemaphore = device::getDevice().createSemaphore(semaphoreInfo);
            renderFinishedSemaphore = device::getDevice().createSemaphore(semaphoreInfo);
            inFlightFence = device::getDevice().createFence(fenceInfo);
        }
        catch(vk::SystemError err)
        {
            LOG_ERROR("failed to create semaphores!");
        }
    
        while (true)
        {
            device::getDevice().waitForFences(1, &inFlightFence, VK_TRUE, UINT64_MAX);
            device::getDevice().resetFences(1, &inFlightFence);

            uint32_t imageIndex;
            imageIndex = device::getDevice().acquireNextImageKHR(swapchain::getSwapChain(id)->swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE).value;
            /* code */
            
            try {
                vk::CommandBufferBeginInfo beginInfo;
                cmdBufferData.vkObject.begin(beginInfo);
                vk::RenderPassBeginInfo renderPassInfo{};
                renderPassInfo.renderPass = renderPassData.vkObject;
                renderPassInfo.framebuffer = framebuffers::getFrameBuffer(swapchain::getSwapChain(id)->swapChainFramebuffers[imageIndex])->fbo;

                renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
                renderPassInfo.renderArea.extent = swapchain::getSwapChain(id)->extent;

                vk::ClearValue clearColor{ std::array<float, 4>{{0.0f, 0.0f, 0.0f, 1.0f}}};
                renderPassInfo.clearValueCount = 1;
                renderPassInfo.pClearValues = &clearColor;

                

                cmdBufferData.vkObject.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

                cmdBufferData.vkObject.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipelineData.graphicsPipeline);

                vk::Viewport viewport{};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = static_cast<float>(swapchain::getSwapChain(id)->extent.width);
                viewport.height = static_cast<float>(swapchain::getSwapChain(id)->extent.height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                cmdBufferData.vkObject.setViewport(0, 1, &viewport);

                vk::Rect2D scissor{};
                scissor.offset = vk::Offset2D{0, 0};
                scissor.extent = swapchain::getSwapChain(id)->extent;
                cmdBufferData.vkObject.setScissor(0, 1, &scissor);
                cmdBufferData.vkObject.draw(3, 1, 0, 0);
                cmdBufferData.vkObject.endRenderPass();
                cmdBufferData.vkObject.end();
            }
            catch (vk::SystemError err) {
                LOG_FATAL("failed to begin recording command buffer! " << err.what());
            }

            vk::SubmitInfo submitInfo;

            vk::Semaphore waitSemaphores[] = {imageAvailableSemaphore};

            vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput};
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &cmdBufferData.vkObject;
            
            vk::Semaphore signalSemaphores[] = {renderFinishedSemaphore};
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;
            try {

                device::getGraphicsQueue().submit(1, &submitInfo, inFlightFence);
            }
            catch (vk::SystemError err) {
                LOG_FATAL("failed to submit draw command buffer!" << err.what());
            }

            vk::PresentInfoKHR presentInfo{};
            presentInfo.waitSemaphoreCount = 0;
            presentInfo.pWaitSemaphores = signalSemaphores;

            vk::SwapchainKHR swapChains[] = {swapchain::getSwapChain(id)->swapChain};
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;

            device::getPresentQueue().presentKHR(&presentInfo);

        }
        

        return id;
    }
    

    void renderer::destroySurface()
    {

    }


    void renderer::renderRequest(const renderRequestInfo& dataToRender)
    {

    }

}