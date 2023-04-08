#include "vulkanRendering.hpp"
#include "vulkanRenderEngine/renderer.hpp"
#include "linuxWindowAPI.hpp"
#include "instance.hpp"
#include "surface.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_wayland.h>

void vulkanRendering::init(entityPool *surfacesPool)
{
    renderData = new vulkanRenderInfoComponent(surfacesPool);
    
    vulkanRenderEngine::renderer::init();
}

void vulkanRendering::close()
{
    vulkanRenderEngine::renderer::close();
    delete renderData;
}


void vulkanRendering::allocateSurfaceToRender(surfaceId winId, void(*callback)(const gpuRenderData&))
{
    vulkanRenderInfo info(callback);
    info.id = winId;
    

    renderData->setComponent(winId, info);

}

void vulkanRendering::setRenderEventListeners(surfaceId winId, void(*callback)(const gpuRenderData&))
{
    renderData->getComponent(winId)->renderFuncion = callback;
}

void vulkanRendering::deallocateSurfaceToRender(surfaceId winId)    
{
    vulkanRenderEngine::renderer::destroySurface();
    renderData->deleteComponent(winId);
}

void vulkanRendering::resize(surfaceId id, int width, int height)
{
    VkWaylandSurfaceCreateInfoKHR surfaceVKData;
    surfaceVKData.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surfaceVKData.display = linuxWindowAPI::display;
    surfaceVKData.surface = surface::getSurface(id);
    surfaceVKData.pNext = nullptr;
    surfaceVKData.flags = 0;

    VkSurfaceKHR vkSurface;
    
    if (vkCreateWaylandSurfaceKHR(vulkanRenderEngine::instance::getInstance(), &surfaceVKData, nullptr, &vkSurface) != VK_SUCCESS) 
    {
        LOG_ERROR("failed to create vulkan surface");
        return;
    }

    renderData->getComponent(id)->swapchain = vulkanRenderEngine::renderer::createSurface((vk::SurfaceKHR)vkSurface, width, height);
}
