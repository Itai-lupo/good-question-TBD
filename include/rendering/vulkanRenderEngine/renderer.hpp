#pragma once 
#include <vulkan/vulkan.hpp>

#include "renderApi.hpp"
#include "gpuRenderData.hpp"

#include "./renderPiplineSystems/textureManger.hpp"
#include "./renderPiplineSystems/vertexArrayManger.hpp"
#include "./renderPiplineSystems/graphicPiplines.hpp"
#include "./renderPiplineSystems/framebuffers.hpp"
#include "./renderPiplineSystems/uniformBufferManger.hpp"
#include "./renderPiplineSystems/renderPasses.hpp"

#include "vkSurfaceComponents.hpp"

class renderApi;

namespace vulkanRenderEngine
{
    class renderer
    {
        private:
            static inline vk::Instance instance{ nullptr };
            static inline vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
            static inline vk::DispatchLoaderDynamic dldi;
            static inline entityPool *vulkanWindosPool;
            static inline vkSurfaceComponents *surfaces;


            static inline bool deviceWasCreated = false;

        public:
            static void init();
            static void close();

            static vkSurfaceId createSurface(vk::SurfaceKHR surfaceToCreate, int width, int height);
            static void destroySurface();

            static void renderRequest(const renderRequestInfo& dataToRender);

    };
    
} 