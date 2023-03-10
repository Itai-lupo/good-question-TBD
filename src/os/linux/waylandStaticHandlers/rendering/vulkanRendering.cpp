#pragma once

#include "vulkanRendering.hpp"
#include "vulkanRenderer.hpp"

void vulkanRendering::init(entityPool *surfacesPool, renderApi *api)
{
    vulkanRenderEngine::vulkanRenderer::init();
}

void vulkanRendering::close()
{

}


void vulkanRendering::allocateSurfaceToRender(surfaceId winId, void(*callback)(const gpuRenderData&))
{

}

void vulkanRendering::setRenderEventListeners(surfaceId winId, void(*callback)(const gpuRenderData&))
{

}


void vulkanRendering::deallocateSurfaceToRender(surfaceId winId);       
{

}

void vulkanRendering::resize(surfaceId id, int width, int height)
{

}
