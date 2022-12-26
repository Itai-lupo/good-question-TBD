#pragma once
#include "core.hpp"
#include "openglContext.hpp"
#include "gpuRenderData.hpp"
#include "entityPool.hpp"
#include "uniformBufferComponents.hpp"

#include <array>
#include <list>
#include <vector>
#include <queue>

namespace openGLRenderEngine
{
    class uniformBuffers
    {
        private:
            static inline openglContext *context;
            static inline uniformBufferComponents *uniformBuffersData;

            static void rebuild(uniformBufferInfo *info);
        public:
            static inline std::queue<uint32_t> toDelete;
            
            static void init(entityPool *uniformBuffersPool);
            static void close();
            
            static void setContext(openglContext *context);
            static void handleRequsets();

            static void setUniformBufferData(uniformBufferInfo info);
            static uniformBufferInfo *getUniformBuffer(uniformBufferId id);
            
            static void bind(uniformBufferId id, int bindingIndex);
    };
}