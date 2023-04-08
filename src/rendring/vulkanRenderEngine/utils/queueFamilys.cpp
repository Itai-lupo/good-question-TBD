#include "queueFamilys.hpp"
#include "log.hpp"

#include <vector>
#include <set>
#include <vulkan/vk_enum_string_helper.h>

namespace vulkanRenderEngine
{
    void queueFamilys::findQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
    {
        std::vector<vk::QueueFamilyProperties> queueFamilysData = device.getQueueFamilyProperties();
        LOG_INFO("There are " << queueFamilysData.size() << " queue families available on the system.")
        int i = 0;
        for(auto& queueFamily: queueFamilysData)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            
            if (presentSupport) 
                presentFamily = i;
            

            LOG_INFO(   "queue type: " <<  vk::to_string(queueFamily.queueFlags) << 
                        " queue count: " << queueFamily.queueCount << 
                        " ");
            if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
                graphicsFamily = i;
            if(queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
                computeFamily = i;
            if(queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)
                transferFamily = i;
            i++;
        }
    }

    std::vector<vk::DeviceQueueCreateInfo> queueFamilys::genrateQueueCreateInfos()
    {
        std::vector<vk::DeviceQueueCreateInfo>queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies{graphicsFamily.value(), presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.setQueueFamilyIndex(queueFamily);
            queueCreateInfo.setQueueCount(1);
            queueCreateInfo.setPQueuePriorities(&queuePriority);
            queueCreateInfos.push_back(queueCreateInfo);
        }

        return queueCreateInfos;
    }


    std::optional<uint32_t> queueFamilys::getGraphicsFamilyQueue()
    {
        return graphicsFamily;
    }

    std::optional<uint32_t> queueFamilys::getPresentFamilyQueue()
    {
        return presentFamily;
    }

    std::optional<uint32_t> queueFamilys::getComputeFamilyQueue()
    {
        return computeFamily;
    }
      
    std::optional<uint32_t> queueFamilys::getTransferFamilyQueue()
    {
        return transferFamily;
    }

    bool queueFamilys::isComplete()
    {
        return graphicsFamily.has_value();
    }
    

}