#include "swapchain.hpp"
#include "device.hpp"
#include "log.hpp"
#include "queueFamilys.hpp"
#include "framebuffers.hpp"

#include <vector>

namespace vulkanRenderEngine
{
    void swapchain::init(entityPool *swapchainsPool)
    {
        swapchainsInfo = new swapchainsComponents(swapchainsPool);
    }

    void swapchain::close()
    {        
        for(int i = 0; i < swapchainsInfo->getData().size(); i++)
        {
            device::getDevice().destroySemaphore(swapchainsInfo->getData()[i].imageAvailableSemaphore);
            device::getDevice().destroySemaphore(swapchainsInfo->getData()[i].renderFinishedSemaphore);
            device::getDevice().destroyFence(swapchainsInfo->getData()[i].inFlightFence);
            
            destroySwapchain(&swapchainsInfo->getData()[i]);
        }

        delete swapchainsInfo;
    }



    void swapchain::create(vkSurfaceId id, const vk::SurfaceKHR& surfaceToCreate, int width, int height)
    {
        swapChainInfo newSwapchain;
        newSwapchain.id = id;
        
        vk::SurfaceCapabilitiesKHR capabilities = device::getPhysicalDevice().getSurfaceCapabilitiesKHR(surfaceToCreate);
        std::vector<vk::SurfaceFormatKHR> formats = device::getPhysicalDevice().getSurfaceFormatsKHR(surfaceToCreate);
        std::vector<vk::PresentModeKHR> presentModes = device::getPhysicalDevice().getSurfacePresentModesKHR(surfaceToCreate);

        

        if(formats.empty() || presentModes.empty()){
            LOG_ERROR("failed to create swapchain: formats is empty || presentModes is empty");
        }

        newSwapchain.formatToUse = choseForamt(formats);
        newSwapchain.presentModeToUse = chosePresentMode(presentModes);

        
        newSwapchain.extent.setWidth(width);
        newSwapchain.extent.setHeight(height);


        vk::SwapchainCreateInfoKHR createInfo = genrateCreateInfo(newSwapchain, surfaceToCreate, capabilities);        
		
        try {
            newSwapchain.swapChain = device::getDevice().createSwapchainKHR(createInfo);
		}
        catch (vk::SystemError err) {
            LOG_ERROR("failed to create swapchain: " << err.what());
		}

        newSwapchain.swapChainImages = device::getDevice().getSwapchainImagesKHR(newSwapchain.swapChain);
        

        newSwapchain.swapChainImagesViews.resize(newSwapchain.swapChainImages.size());

        for (size_t i = 0; i < newSwapchain.swapChainImages.size(); i++) {
            vk::ImageViewCreateInfo createInfo{};
            createInfo.setViewType(vk::ImageViewType::e2D);
            createInfo.setFormat(newSwapchain.formatToUse.format);
            createInfo.setImage(newSwapchain.swapChainImages[i]);

            createInfo.components.setR(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setG(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setB(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setA(vk::ComponentSwizzle::eIdentity);

            createInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
            createInfo.subresourceRange.setBaseMipLevel(0);
            createInfo.subresourceRange.setLevelCount(1);
            createInfo.subresourceRange.setBaseArrayLayer(0);
            createInfo.subresourceRange.setLayerCount(1);

            try
            {
                newSwapchain.swapChainImagesViews[i] = device::getDevice().createImageView(createInfo, nullptr);
            }
            catch(vk::SystemError err) 
            {
                LOG_ERROR("failed to create image view: " << err.what());
            }
        }

        swapchainsInfo->setComponent(id, newSwapchain);
    }

    void swapchain::resize(vkSurfaceId id, const vk::SurfaceKHR& surfaceToCreate, int width, int height)
    {
        destroySwapchain(swapchainsInfo->getComponent(id));

        swapChainInfo newSwapchain;
        vk::SurfaceCapabilitiesKHR capabilities = device::getPhysicalDevice().getSurfaceCapabilitiesKHR(surfaceToCreate);
        std::vector<vk::SurfaceFormatKHR> formats = device::getPhysicalDevice().getSurfaceFormatsKHR(surfaceToCreate);
        std::vector<vk::PresentModeKHR> presentModes = device::getPhysicalDevice().getSurfacePresentModesKHR(surfaceToCreate);

        

        if(formats.empty() || presentModes.empty()){
            LOG_ERROR("failed to create swapchain: formats is empty || presentModes is empty");
        }

        newSwapchain.formatToUse = choseForamt(formats);
        newSwapchain.presentModeToUse = chosePresentMode(presentModes);

        
        newSwapchain.extent.setWidth(width);
        newSwapchain.extent.setHeight(height);


        vk::SwapchainCreateInfoKHR createInfo = genrateCreateInfo(newSwapchain, surfaceToCreate, capabilities);        
		
        try {
            newSwapchain.swapChain = device::getDevice().createSwapchainKHR(createInfo);
            newSwapchain.swapChainImages = device::getDevice().getSwapchainImagesKHR(newSwapchain.swapChain);
            

            newSwapchain.swapChainImagesViews.resize(newSwapchain.swapChainImages.size());
		}
        catch (vk::SystemError err) {
            LOG_ERROR("failed to create swapchain: " << err.what());
		}


        for (size_t i = 0; i < newSwapchain.swapChainImages.size(); i++) {
            vk::ImageViewCreateInfo createInfo{};
            createInfo.setViewType(vk::ImageViewType::e2D);
            createInfo.setFormat(newSwapchain.formatToUse.format);
            createInfo.setImage(newSwapchain.swapChainImages[i]);

            createInfo.components.setR(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setG(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setB(vk::ComponentSwizzle::eIdentity);
            createInfo.components.setA(vk::ComponentSwizzle::eIdentity);

            createInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
            createInfo.subresourceRange.setBaseMipLevel(0);
            createInfo.subresourceRange.setLevelCount(1);
            createInfo.subresourceRange.setBaseArrayLayer(0);
            createInfo.subresourceRange.setLayerCount(1);

            try
            {
                newSwapchain.swapChainImagesViews[i] = device::getDevice().createImageView(createInfo, nullptr);
            }
            catch(vk::SystemError err) 
            {
                LOG_ERROR("failed to create image view: " << err.what());
            }
            swapchainsInfo->setComponent(id, newSwapchain);

        }

    }

    void swapchain::destroy(vkSurfaceId id)
    {
        
        destroySwapchain(swapchainsInfo->getComponent(id));
        swapchainsInfo->deleteComponent(id);
    }

    swapChainInfo *swapchain::getSwapChain(vkSurfaceId id)
    {
        // LOG_INFO(id.index << ", " << (int)id.gen)
        return swapchainsInfo->getComponent(id);
    }


    void swapchain::logSurfaceCapabilities(vk::SurfaceCapabilitiesKHR capabilities)
    {
        LOG_INFO("Swapchain can support the following surface capabilities:");

        LOG_INFO("minimum image count: " << capabilities.minImageCount);
        LOG_INFO("maximum image count: " << capabilities.maxImageCount);

        LOG_INFO("current extent: ")
        LOG_INFO("\twidth: " << capabilities.currentExtent.width);
        LOG_INFO("\theight: " << capabilities.currentExtent.height);

        LOG_INFO("minimum supported extent: ");
        LOG_INFO("\twidth: " << capabilities.minImageExtent.width);
        LOG_INFO("\theight: " << capabilities.minImageExtent.height);

        LOG_INFO("maximum supported extent: ");
        LOG_INFO("\twidth: " << capabilities.maxImageExtent.width);
        LOG_INFO("\theight: " << capabilities.maxImageExtent.height);

        LOG_INFO("maximum image array layers: " << capabilities.maxImageArrayLayers);
    }

    vk::SurfaceFormatKHR swapchain::choseForamt(std::vector<vk::SurfaceFormatKHR>& formats)
    {
        for (vk::SurfaceFormatKHR format : formats)
			if (format.format == vk::Format::eB8G8R8A8Unorm	&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                    return format;
        return formats[0];
    }

    vk::PresentModeKHR swapchain::chosePresentMode(std::vector<vk::PresentModeKHR>& presentModes)
    {
        for (const auto& availablePresentMode : presentModes) 
            if (availablePresentMode == vk::PresentModeKHR::eMailbox)
                return availablePresentMode;
        return vk::PresentModeKHR::eFifo;
    }

    vk::SwapchainCreateInfoKHR swapchain::genrateCreateInfo(swapChainInfo& swapChainData, const vk::SurfaceKHR& surfaceToCreate, vk::SurfaceCapabilitiesKHR& capabilities)
    {
        uint32_t imageCount = capabilities.minImageCount + 1;
        vk::SwapchainCreateInfoKHR createInfo;

        createInfo.setSurface(surfaceToCreate);
        createInfo.setMinImageCount(imageCount);
        createInfo.setImageFormat(swapChainData.formatToUse.format);
        createInfo.setImageColorSpace(swapChainData.formatToUse.colorSpace);
        createInfo.setImageExtent(swapChainData.extent);
        createInfo.setImageArrayLayers(1);
        createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);


        uint32_t queueFamilyIndices[] =    {queueFamilys::getGraphicsFamilyQueue().value() , queueFamilys::getPresentFamilyQueue().value()}; 
        if (queueFamilys::getGraphicsFamilyQueue().value() != queueFamilys::getPresentFamilyQueue().value()) {
            createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
            createInfo.setQueueFamilyIndexCount(2);
            createInfo.setPQueueFamilyIndices(queueFamilyIndices);
        } else {
            createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
            createInfo.setQueueFamilyIndexCount(0);
            createInfo.setPQueueFamilyIndices(nullptr);
        }

        createInfo.setPreTransform(capabilities.currentTransform);
        createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        createInfo.setPresentMode(swapChainData.presentModeToUse);
        createInfo.setClipped(VK_TRUE);
        createInfo.setOldSwapchain(VK_NULL_HANDLE);

        return createInfo;
    }

    void swapchain::destroySwapchain(swapChainInfo *toDestroy)
    {
        for(int i = 0; i < toDestroy->swapChainFramebuffers.size(); i++)
            framebuffers::destroy(toDestroy->swapChainFramebuffers[i]);
        for (auto imageView : toDestroy->swapChainImagesViews) 
            device::getDevice().destroyImageView(imageView, nullptr);
        device::getDevice().destroySwapchainKHR(toDestroy->swapChain);

    }



}