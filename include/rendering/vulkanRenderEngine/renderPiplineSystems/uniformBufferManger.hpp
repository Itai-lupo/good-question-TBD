#pragma once
#include "core.hpp"
#include "gpuRenderData.hpp"
#include "entityPool.hpp"
#include "renderComponentTemplate.hpp"

#include <array>
#include <list>
#include <vector>
#include <queue>

struct ubo
{
    uniformBufferId id;
};

namespace vulkanRenderEngine
{
    class UBOs
    {
    private:
        static inline entityPool *UBOsPool;
        static inline renderComponentTemplate *UBOsData;

        static void buildPool();

    public:
        static inline std::queue<uint32_t> toDelete;

        static void init(entityPool *UBOsPool) noexcept;
        static void close() noexcept;

        static vaoId createUniformBufferData(ubo info);
        static void setUniformBufferData(ubo info) noexcept;
        static ubo &getUniformBuffer(uniformBufferId id) noexcept;
    };
}