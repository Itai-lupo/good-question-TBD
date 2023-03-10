#pragma once 
#include <vulkan/vulkan.hpp>

namespace vulkanRenderEngine
{
    class renderer
    {
        private:
            //instance-related variables
            vk::Instance instance{ nullptr };
            vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
            vk::DispatchLoaderDynamic dldi;
            vk::SurfaceKHR surface;

        public:
            static void init();
            static void close();
    }
    
} 