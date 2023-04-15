#pragma once
#include "core.hpp"
#include "entityPool.hpp"
#include "swapchainsComponents.hpp"

#include <vulkan/vulkan.hpp>


namespace vulkanRenderEngine
{
    
    class swapchain
    {
        private:

            static void logSurfaceCapabilities(vk::SurfaceCapabilitiesKHR c);
            static vk::SurfaceFormatKHR choseForamt(std::vector<vk::SurfaceFormatKHR>& formats);
            static vk::PresentModeKHR chosePresentMode(std::vector<vk::PresentModeKHR>& presentModes);
            static vk::SwapchainCreateInfoKHR genrateCreateInfo(swapChainInfo& swapChainData, const vk::SurfaceKHR& surfaceToCreate, vk::SurfaceCapabilitiesKHR& capabilities);

            static void destroySwapchain(swapChainInfo *toDestroy);
        public:
            static inline swapchainsComponents *swapchainsInfo;
            
            static void init(entityPool *swapchainsPool);
            static void close();
            static void create(vkSurfaceId id, const vk::SurfaceKHR& surfaceToCreate, int width, int height); 
            static void resize(vkSurfaceId id, int width, int height); 
            
            static void destroy(vkSurfaceId id); 

            static swapChainInfo *getSwapChain(vkSurfaceId id); 
    };
}
