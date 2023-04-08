#include <vulkan/vk_enum_string_helper.h>

#include <vector>

#include "instance.hpp"
#include "log.hpp"

namespace vulkanRenderEngine
{

    void instance::create()
    {
        

        
		uint32_t version{ 0 };
		vkEnumerateInstanceVersion(&version);

        // LOG_INFO(   VK_API_VERSION_VARIANT(version) << ", " <<
        //             VK_API_VERSION_MAJOR(version) << ", " <<
        //             VK_API_VERSION_MINOR(version) << ", " <<
        //             VK_API_VERSION_PATCH(version));


        vk::ApplicationInfo appInfo = vk::ApplicationInfo(
                    "new tale editor",
                    VK_MAKE_VERSION(0, 1, 0),
                    "new tale",
                    VK_MAKE_VERSION(0, 1, 0),
                    version
                );
        
        uint32_t extensions_count = 0;
        VkResult result = VK_SUCCESS;
        

        // std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
        
        // for(auto& ext: availableLayers)
        // {
        //     LOG_INFO(ext.layerName << ", " << ext.description << ", " << ext.implementationVersion);
        // }


// VK_LAYER_LUNARG_screenshot, LunarG image capture layer, 1
// VK_LAYER_LUNARG_monitor, Execution Monitoring Layer, 1
// VK_LAYER_LUNARG_api_dump, LunarG API dump layer, 2
// VK_LAYER_KHRONOS_validation, Khronos Validation Layer, 1


        // std::vector<vk::ExtensionProperties> temp = vk::enumerateInstanceExtensionProperties();
        
        // for(auto& ext: temp)
        // {
        //     LOG_INFO(ext.extensionName << ", " << ext.specVersion);
        // }

// VK_KHR_device_group_creation, 1
// VK_KHR_display, 23

// VK_KHR_external_fence_capabilities, 1
// VK_KHR_external_memory_capabilities, 1
// VK_KHR_external_semaphore_capabilities, 1

// VK_KHR_get_display_properties2, 1
// VK_KHR_get_physical_device_properties2, 2
// VK_KHR_get_surface_capabilities2, 1
// VK_KHR_surface, 25
// VK_KHR_surface_protected_capabilities, 1
// VK_KHR_wayland_surface, 6
// VK_KHR_xcb_surface, 6
// VK_KHR_xlib_surface, 6
// VK_EXT_acquire_drm_display, 1
// VK_EXT_acquire_xlib_display, 1
// VK_EXT_debug_report, 10
// VK_EXT_debug_utils, 2
// VK_EXT_direct_mode_display, 1
// VK_EXT_display_surface_counter, 1
// VK_KHR_portability_enumeration, 1

        std::vector<const char*> extensions
        {
            "VK_KHR_display",
            "VK_KHR_surface",
            "VK_KHR_wayland_surface",
            "VK_EXT_acquire_drm_display",
            "VK_EXT_debug_report",
            "VK_EXT_debug_utils",
            "VK_EXT_direct_mode_display",
            "VK_EXT_display_surface_counter",
            "VK_KHR_portability_enumeration"
        };

        std::vector<const char*> layers
        {
            "VK_LAYER_KHRONOS_validation"
        };

        

        vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&appInfo,
			layers.size(), layers.data(),
			extensions.size(), extensions.data()
		);

        

        try {
			vkInstanceData = vk::createInstance(createInfo);
            dldi = vk::DispatchLoaderDynamic(vkInstanceData, vkGetInstanceProcAddr);
            LOG_INFO("created vk Instance")
		}
		catch (vk::SystemError err) {
            LOG_ERROR("failed to create vk instance with  error code: " << err.what());
		}
    }

    void instance::destroy()
    {
        vkInstanceData.destroy();
    }

    vk::Instance& instance::getInstance()
    {
        return vkInstanceData;
    }

    vk::DispatchLoaderDynamic& instance::getLoader()
    {
        return dldi;
    }

}