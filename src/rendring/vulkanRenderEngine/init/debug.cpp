#include "debug.hpp"
#include "instance.hpp"
#include "log.hpp"

namespace vulkanRenderEngine
{
    VkBool32 debug::callback(
                                VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
                                VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
                                const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
                                void*                                            pUserData)
    {
        switch (messageSeverity)
        {

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                LOG_FATAL("vk FATAL error msg: " << pCallbackData->pMessage);
            break;
                
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                LOG_ERROR("vk ERROR msg: " << pCallbackData->pMessage );

            break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                LOG_WARNING("vk WARNING msg: " << pCallbackData->pMessage );

            break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                LOG_INFO("vk INFO msg" << pCallbackData->pMessage);

            break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                LOG_INFO("vk VERBOSE msg: " << pCallbackData->pMessage);
            break;
        
        default:
            LOG_FATAL("something went very wrong messageSeverity = " << messageSeverity << ", msg" << pCallbackData->pMessage);
            break;
        }
        return VK_FALSE;

    }

    void debug::setup()
    {        
        vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
            vk::DebugUtilsMessengerCreateFlagsEXT(),
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            callback,
            nullptr
        );

        debugMessenger = instance::getInstance().createDebugUtilsMessengerEXT(createInfo, nullptr, instance::getLoader());
    } 

    void debug::close()
    {
        instance::getInstance().destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, instance::getLoader());
    }

}