#pragma once
#include <vulkan/vulkan.hpp>


namespace vulkanRenderEngine
{
    class instance
    {
        private:
            static inline vk::Instance vkInstanceData;
            static inline vk::DispatchLoaderDynamic dldi;
        public:
            static void create();
            static void destroy();

            static vk::Instance& getInstance();
            static vk::DispatchLoaderDynamic& getLoader();
    };
    
}