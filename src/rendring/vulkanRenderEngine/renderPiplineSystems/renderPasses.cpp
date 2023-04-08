#include "vulkanRenderEngine/renderPiplineSystems/renderPasses.hpp"
#include "device.hpp"
#include "log.hpp"

namespace vulkanRenderEngine
{
    void renderPasses::init(entityPool *renderPassesPool)
    {
        renderPasses::renderPassesPool = new entityPool(350);// renderPassesPool;
        renderPassesData = new renderPassesComponents(renderPasses::renderPassesPool);
    }

    void renderPasses::close()
    {
        for(int i = 0; i < renderPassesData->getData().size(); i++)
            destroyRenderPass(&renderPassesData->getData()[i]);

        delete renderPassesData;
    }

    renderPassId renderPasses::create(renderPassInfo& data)
    {
        renderPassId id = renderPassesPool->allocEntity();
        data.id = id;
        createRenderPass(data);
        renderPassesData->setComponent(id, data);
        return id;
    }

    void renderPasses::set(renderPassInfo& data)
    {
        createRenderPass(data);
        renderPassesData->setComponent(data.id, data);
    }


    renderPassInfo *renderPasses::get(renderPassId id)
    {
        return renderPassesData->getComponent(id);
    }

    void renderPasses::destroy(renderPassId id)
    {
        destroyRenderPass(renderPassesData->getComponent(id));

        renderPassesData->deleteComponent(id);
    }





 

    void renderPasses::createRenderPass(renderPassInfo& info) {
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.format = info.format;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass{};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
       
        vk::SubpassDependency dependency;
        dependency.srcSubpass =  VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.srcAccessMask = vk::AccessFlagBits::eNone;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        try {
			info.vkObject = device::getDevice().createRenderPass(renderPassInfo);
		}
		catch (vk::SystemError err) {
            LOG_FATAL("Failed to create render pass: " << err.what());
		}


    }


    void renderPasses::destroyRenderPass(renderPassInfo* info) 
    {
        device::getDevice().destroyRenderPass(info->vkObject);   
    }
}