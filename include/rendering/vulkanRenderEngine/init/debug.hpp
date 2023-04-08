#pragma once
#include <vulkan/vulkan.hpp>


namespace vulkanRenderEngine
{
    class debug
    {
        private:
            static inline vk::DebugUtilsMessengerEXT debugMessenger;

            static VkBool32 callback(
                                VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
                                VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
                                const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
                                void*                                            pUserData); 
                                
        public:
            static void setup();
            static void close();
    };
    
}