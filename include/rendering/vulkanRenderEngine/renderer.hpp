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
#include "./renderPiplineSystems/commandBuffers.hpp"

#include "vkSurfaceComponents.hpp"

#include <thread>
#include "safeQueue.hpp"

class renderApi;

struct drawRequst
{
    vkSurfaceId surfaceId;
    commandBufferId cmdId;
    void (*callback)(vkSurfaceId surfaceId, commandBufferId cmdId, uint32_t imageIndex);
};

struct presentRequst
{
    vkSurfaceId surfaceId;
    commandBufferId cmdId;
    void (*callback)(vkSurfaceId surfaceId, commandBufferId cmdId, uint32_t imageIndex);
    vk::SubmitInfo submitInfo;
    uint32_t imageIndex;
};

namespace vulkanRenderEngine
{
    class renderer
    {
    private:
        static inline vk::Instance instance{nullptr};
        static inline vk::DebugUtilsMessengerEXT debugMessenger{nullptr};
        static inline vk::DispatchLoaderDynamic dldi;
        static inline entityPool *vulkanWindosPool;
        static inline vkSurfaceComponents *surfaces;

        static inline bool deviceWasCreated = false;
        static inline bool isAlive = false;

        static void drawLoop();
        static void handleAllTranferRequestes();
        static void handleRenderRequest();
        static void handlePresentRequset();

        static void recreateSwapChain(vkSurfaceId id);

        static void initDevice(vk::SurfaceKHR surfaceToCreate);
        static inline SafeQueue<presentRequst> presentRequsts;
        static inline std::thread *drawThread;

        static inline std::mutex m;
        static inline commandPoolId poolId;

    public:
        static inline SafeQueue<drawRequst> drawRequsts;
        static void init();
        static void close();

        static vkSurfaceId createSurface(vk::SurfaceKHR surfaceToCreate, int width, int height);
        static void destroySurface();

        static void resize(vkSurfaceId id, int width, int height);

        static bool isSurfaceValid(vkSurfaceId id);

        static void renderRequest(const renderRequestInfo &dataToRender);
    };

    ;

}