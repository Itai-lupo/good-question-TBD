#pragma once
#include <vulkan/vulkan.hpp>


namespace vulkanRenderEngine
{
    
    class device
    {
        private:
            static inline vk::PhysicalDevice physicalDevice;
            static inline vk::Device logicalDevice;

            static inline vk::Queue graphicsQueue;
            static inline vk::Queue presentQueue;
            static inline vk::Queue computeQueue;
            static inline vk::Queue transferQueue;
            
            static void choosePhysicalDevice();
            static void createLogicalDevice();

            static void logDeviceProperties(const vk::PhysicalDevice& device);
            static int rateDeviceSuitability(const vk::PhysicalDevice& device);
            


        public:
            static void create(const vk::SurfaceKHR& surface);
            static void destroy();
            
            static vk::PhysicalDevice& getPhysicalDevice();
            static vk::Device& getDevice();


            static vk::Queue getGraphicsQueue();
            static vk::Queue getPresentQueue();
            static vk::Queue getComputeQueue();
            static vk::Queue getTransferQueue();


    };
}