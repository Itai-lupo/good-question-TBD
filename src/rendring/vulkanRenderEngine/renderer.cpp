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
        graphicPiplines::init(new entityPool(256));
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
            LOG_INFO("vulkan renderer init successfully")
            drawThread = new std::thread(drawLoop);
            presentThread = new std::thread(presentLoop);
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
   
        for(int i = 0; i < swapchain::getSwapChain(id)->swapChainImagesViews.size(); i++)
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
        catch(vk::SystemError err)
        {
            LOG_ERROR("failed to create semaphores!");
        }
    
        commandBufferInfo cmdBufferData;
        
        swapchain::getSwapChain(id)->renderPass = renderPass;
        swapchain::getSwapChain(id)->graphicPiplineId = graphicPiplineId;
        swapchain::getSwapChain(id)->cmdId = commandBuffers::create(cmdBufferData);
        swapchain::getSwapChain(id)->complete = true;
        
        drawRequsts.enqueue({id});

        return id;
    }
    

    void renderer::destroySurface()
    {

    }


    void renderer::renderRequest(const renderRequestInfo& dataToRender)
    {

    }

    bool renderer::isSurfaceValid(vkSurfaceId id)
    {
        return vulkanWindosPool->isIdValid(id);
    }


    void renderer::drawLoop()
    {
        while (true)
        {
            drawRequst temp = drawRequsts.dequeue();
            swapChainInfo *swapchainData =  swapchain::getSwapChain(temp.surfaceId);
            
            device::getDevice().waitForFences(1, &swapchainData->inFlightFence, VK_TRUE, UINT64_MAX);

            auto [res, imageIndex] = device::getDevice().acquireNextImageKHR(swapchainData->swapChain, UINT64_MAX, swapchainData->imageAvailableSemaphore, VK_NULL_HANDLE);
            if (res == vk::Result::eErrorOutOfDateKHR ) {
                recreateSwapChain(swapchainData->id);
                break;

            } else if (res !=  vk::Result::eSuccess && res !=  vk::Result::eSuboptimalKHR) {
                LOG_FATAL("failed to submit draw command buffer!");
            }

            device::getDevice().resetFences(1, &swapchainData->inFlightFence);
            commandBufferInfo cmdBufferData = *commandBuffers::get(swapchainData->cmdId);

            try {
                vk::CommandBufferBeginInfo beginInfo;
                cmdBufferData.vkObject.begin(beginInfo);
                vk::RenderPassBeginInfo renderPassCreateInfo{};
                
                renderPassCreateInfo.renderPass = renderPasses::get(swapchainData->renderPass)->vkObject;
                renderPassCreateInfo.framebuffer = framebuffers::getFrameBuffer(swapchainData->swapChainFramebuffers[imageIndex])->fbo;

                renderPassCreateInfo.renderArea.offset = vk::Offset2D{0, 0};
                renderPassCreateInfo.renderArea.extent = swapchainData->extent;

                vk::ClearValue clearColor{ std::array<float, 4>{{0.0f, 0.0f, 0.0f, 1.0f}}};
                renderPassCreateInfo.clearValueCount = 1;
                renderPassCreateInfo.pClearValues = &clearColor;

                

                cmdBufferData.vkObject.beginRenderPass(renderPassCreateInfo, vk::SubpassContents::eInline);

                cmdBufferData.vkObject.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicPiplines::get(swapchainData->graphicPiplineId)->graphicsPipeline);

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
                cmdBufferData.vkObject.draw(3, 1, 0, 0);
                cmdBufferData.vkObject.endRenderPass();
                cmdBufferData.vkObject.end();
            }
            catch (vk::SystemError err) {
                LOG_FATAL("failed to begin recording command buffer! " << err.what());
            }

            vk::SubmitInfo submitInfo;

            vk::Semaphore waitSemaphores[] = {swapchainData->imageAvailableSemaphore};

            vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput};
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &cmdBufferData.vkObject;
            
            vk::Semaphore signalSemaphores[] = {swapchainData->renderFinishedSemaphore};
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;
            try {
                vk::Result  r = device::getGraphicsQueue().submit(1, &submitInfo, swapchainData->inFlightFence);
                LOG_INFO(vk::to_string(r) << ", " << temp.surfaceId.index  << ", " << (int)temp.surfaceId.gen);
                presentRequsts.enqueue({temp.surfaceId, submitInfo, imageIndex});
            }
            catch (vk::SystemError err) {
                LOG_FATAL("failed to submit draw command buffer!" << err.what());
            }

        }

    }

    void renderer::presentLoop()
    {
        while (true)
        {
            presentRequst temp = presentRequsts.dequeue();
            device::getDevice().waitIdle();

            swapChainInfo *swapchainData = swapchain::getSwapChain(temp.surfaceId);
            vk::SubmitInfo submitInfo = temp.submitInfo;

            vk::Semaphore signalSemaphores[] = {swapchainData->renderFinishedSemaphore};

            vk::PresentInfoKHR presentInfo{};
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;

            if(swapchainData == nullptr)
            {
                break;
            }

            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &swapchainData->swapChain;
            presentInfo.pImageIndices = &temp.imageIndex;
            
            vk::Result  r = device::getPresentQueue().presentKHR(&presentInfo);
            LOG_INFO(vk::to_string(r) << ", " << temp.surfaceId.index  << ", " << (int)temp.surfaceId.gen);
            if (r == vk::Result::eErrorOutOfDateKHR || r ==  vk::Result::eSuboptimalKHR || swapchainData->outOfData) {
                recreateSwapChain(swapchainData->id);
                
            } else if (r !=  vk::Result::eSuccess) {
                LOG_FATAL("failed to submit draw command buffer!");
            }
            
            drawRequsts.enqueue({temp.surfaceId});


        }
    }


    void renderer::resize(vkSurfaceId id, int width, int height)
    {
        if(swapchain::swapchainsInfo->getComponent(id))
        {
            swapchain::swapchainsInfo->getComponent(id)->outOfData = true;
            swapchain::swapchainsInfo->getComponent(id)->extent.height = height;
            swapchain::swapchainsInfo->getComponent(id)->extent.width =  width;
        }
    }

    void renderer::recreateSwapChain(vkSurfaceId id)
    {
        LOG_INFO(id.index << ", " << id.gen);
        device::getDevice().waitIdle();
        swapChainInfo temp = *swapchain::getSwapChain(id);
        
        swapchain::destroy(id);
        
        swapchain::create(id, surfaces->getComponent(id), temp.extent.width, temp.extent.height);
        for(int i = 0; i < swapchain::getSwapChain(id)->swapChainImagesViews.size(); i++)
        {
            swapchain::getSwapChain(id)->swapChainFramebuffers.push_back(
            framebuffers::createFrameBuffer(
                temp.renderPass, 
                &swapchain::getSwapChain(id)->swapChainImagesViews[i],
                temp.extent.width,
                temp.extent.height));
        }

        

        swapchain::getSwapChain(id)->imageAvailableSemaphore = temp.imageAvailableSemaphore;
        swapchain::getSwapChain(id)->renderFinishedSemaphore = temp.renderFinishedSemaphore;
        swapchain::getSwapChain(id)->inFlightFence = temp.inFlightFence;
                
        swapchain::getSwapChain(id)->renderPass = temp.renderPass;
        swapchain::getSwapChain(id)->graphicPiplineId = temp.graphicPiplineId;
        swapchain::getSwapChain(id)->cmdId = temp.cmdId;
        swapchain::getSwapChain(id)->complete = true;

    }

}