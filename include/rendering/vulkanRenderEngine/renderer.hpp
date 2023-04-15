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


#include <thread>
#include "safeQueue.hpp"

class renderApi;

struct drawRequst
{
    vkSurfaceId surfaceId;
};


struct presentRequst
{
    vkSurfaceId surfaceId;
    vk::SubmitInfo submitInfo;
    uint32_t imageIndex;
};


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

            static void drawLoop();
            static void presentLoop();

            static void recreateSwapChain(vkSurfaceId id);

            static inline SafeQueue<drawRequst> drawRequsts;
            static inline std::thread *drawThread;


            static inline SafeQueue<presentRequst> presentRequsts;
            static inline std::thread *presentThread;
            
        public:
            static void init();
            static void close();

            static vkSurfaceId createSurface(vk::SurfaceKHR surfaceToCreate, int width, int height);
            static void destroySurface();

            static void resize(vkSurfaceId id, int width, int height);

            static bool isSurfaceValid(vkSurfaceId id);

            static void renderRequest(const renderRequestInfo& dataToRender);

    };
    
} 