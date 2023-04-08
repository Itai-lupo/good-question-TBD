#include "device.hpp"
#include "queueFamilys.hpp"

#include <vulkan/vk_enum_string_helper.h>

#include <map>
#include <set>
#include <vector>

#include "log.hpp"
#include "instance.hpp"


namespace vulkanRenderEngine
{

    void device::create(const vk::SurfaceKHR& surface)
    {
        choosePhysicalDevice();
        queueFamilys::findQueueFamilies(physicalDevice, surface);
        createLogicalDevice();
    }

    void device::destroy()
    {
        logicalDevice.destroy();
    }

            
    vk::PhysicalDevice& device::getPhysicalDevice()
    {
        return physicalDevice;
    }

    vk::Device& device::getDevice()
    {
        return logicalDevice;
    }

    vk::Queue device::getGraphicsQueue()
    {
        return graphicsQueue;
    }

    vk::Queue device::getPresentQueue()
    {
        return presentQueue;
    }

    vk::Queue device::getComputeQueue()
    {
        return computeQueue;
    }

    vk::Queue device::getTransferQueue()
    {
        return transferQueue;
    }



    void device::choosePhysicalDevice()
    {

        std::vector<vk::PhysicalDevice> availableDevices = instance::getInstance().enumeratePhysicalDevices();
        std::multimap<int, VkPhysicalDevice> candidates;

        if(availableDevices.size() == 0)
        {
            LOG_ERROR("There are no physical devices that support vulkan available on this system");
            return;
        }

		LOG_INFO("There are " << availableDevices.size() << " physical devices available on this system");

		for (const auto& device: availableDevices) {

            logDeviceProperties(device);

            candidates.insert(std::make_pair(rateDeviceSuitability(device), device));
		}

        if (candidates.rbegin()->first > 0) {
            physicalDevice = candidates.rbegin()->second;
        } else {
            LOG_ERROR("failed to find a suitable GPU!");
        }
        

    }

    void device::createLogicalDevice()
    {
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.setQueueFamilyIndex(queueFamilys::getGraphicsFamilyQueue().value());
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.setFlags(vk::DeviceQueueCreateFlags());

            float queuePriority = 1.0f;
            queueCreateInfo.setPQueuePriorities(&queuePriority);

            vk::PhysicalDeviceFeatures deviceFeatures = physicalDevice.getFeatures();
            
            std::vector<const char*> deviceExtensions = {
                        VK_KHR_SWAPCHAIN_EXTENSION_NAME
                    };
    		std::vector<const char*> enabledLayers;
            enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
            
            std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos =  queueFamilys::genrateQueueCreateInfos();

            vk::DeviceCreateInfo createInfo;
            createInfo.setPQueueCreateInfos(&queueCreateInfo);
            createInfo.setQueueCreateInfoCount(1);
            createInfo.setPEnabledFeatures(&deviceFeatures);
            createInfo.setFlags(vk::DeviceCreateFlags());
            createInfo.setEnabledLayerCount(enabledLayers.size());
            createInfo.setPpEnabledLayerNames(enabledLayers.data());
            createInfo.setEnabledExtensionCount(deviceExtensions.size());
            createInfo.setPpEnabledExtensionNames( deviceExtensions.data());
            createInfo.queueCreateInfoCount = queueCreateInfos.size();
            createInfo.pQueueCreateInfos = queueCreateInfos.data();

            
		try {
			logicalDevice = physicalDevice.createDevice(createInfo);
            graphicsQueue = logicalDevice.getQueue(queueFamilys::getGraphicsFamilyQueue().value(), 0);
            presentQueue = logicalDevice.getQueue(queueFamilys::getPresentFamilyQueue().value(), 0);
            computeQueue = logicalDevice.getQueue(queueFamilys::getComputeFamilyQueue().value(), 0);
            transferQueue = logicalDevice.getQueue(queueFamilys::getTransferFamilyQueue().value(), 0);
		}
		catch (vk::SystemError err) {
            LOG_ERROR("failed to create vulkan device: " << err.what());
		}

    }

    void device::logDeviceProperties(const vk::PhysicalDevice& device) {
		
		vk::PhysicalDeviceProperties properties = device.getProperties();

		LOG_INFO("Device name: " << properties.deviceName);
        
		LOG_INFO("Device type: " << vk::to_string(properties.deviceType));
	}

    int device::rateDeviceSuitability(const vk::PhysicalDevice& device)
    {
        vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
        vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

        std::set<std::string> requestedExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

        std::vector<vk::ExtensionProperties> supportedExtensions = device.enumerateDeviceExtensionProperties();
        
        for(const auto& ext: supportedExtensions)
        {
            // LOG_INFO(ext.extensionName)
            requestedExtensions.erase(ext.extensionName);
        }

        if(requestedExtensions.size() != 0)
            return -1;


        int score = 1;

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        // Application can't function without geometry shaders
        if (!deviceFeatures.geometryShader) {
            return 0;
        }

        return score;
    }



}
