#pragma once 

#include "core.hpp"
#include "entityPool.hpp"

#include <array>
#include <vulkan/vulkan.hpp>

struct swapChainInfo
{
    vkSurfaceId id;
    vk::SurfaceFormatKHR formatToUse;
    vk::PresentModeKHR presentModeToUse;
    vk::Extent2D extent;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::ImageView> swapChainImagesViews;
    vk::SwapchainKHR swapChain;
    std::vector<framebufferId> swapChainFramebuffers;
    
};


class swapchainsComponents 
{
    private:
        entityPool *pool;

        std::vector<swapChainInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            swapchainsComponents  *This = static_cast<swapchainsComponents *>(data);
            This->deleteComponent(id);
        }

    public:
        swapchainsComponents (entityPool *pool);
        ~swapchainsComponents ();

        void deleteComponent(entityId id);
        swapChainInfo *getComponent(entityId id);
        void setComponent(entityId id, swapChainInfo& buffer);
        std::vector<swapChainInfo>& getData()
        {
            return data;
        }
};
