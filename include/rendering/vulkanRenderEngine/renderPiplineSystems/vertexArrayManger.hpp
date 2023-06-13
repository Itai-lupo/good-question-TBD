#pragma once
#include "core.hpp"
#include "log.hpp"

#include "entityPool.hpp"
#include "renderComponentTemplate.hpp"
#include "vaoData.hpp"

#include <list>
#include <vector>
#include <queue>
#include <vulkan/vulkan.hpp>

namespace vulkanRenderEngine
{

    class vaos
    {
    private:
        static inline entityPool *vaosPool;
        static inline renderComponentTemplate *vaosData;

    public:
        static void init(entityPool *vaosPool) noexcept;
        static void close() noexcept;

        static vaoId createVAO(vao info) noexcept;
        static void setVAOData(vao info) noexcept;
        static vao &getVAO(vaoId id) noexcept;

        static vk::VertexInputBindingDescription *getBindingDescriptions(vaoId id) noexcept;
        static vk::VertexInputAttributeDescription *getAttributeDescriptions(vaoId id) noexcept;

        static size_t getBindingDescriptionsSize(vaoId) noexcept;
        static size_t getAttributeDescriptionsSize(vaoId) noexcept;
    };
}
