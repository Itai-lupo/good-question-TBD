#pragma once

#include <vulkan/vulkan.hpp>

#include <optional>
#include <vector>

namespace vulkanRenderEngine
{
    
    class queueFamilys
    {
        private:
            static inline std::optional<uint32_t> graphicsFamily;
		    static inline std::optional<uint32_t> presentFamily;

            static inline std::optional<uint32_t> computeFamily;
		    static inline std::optional<uint32_t> transferFamily;


        public:

            static void findQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
            static std::vector<vk::DeviceQueueCreateInfo> genrateQueueCreateInfos();

            static std::optional<uint32_t> getGraphicsFamilyQueue();      
            static std::optional<uint32_t> getPresentFamilyQueue();   
            static std::optional<uint32_t> getComputeFamilyQueue();      
            static std::optional<uint32_t> getTransferFamilyQueue();      

            static bool isComplete();

    };
}