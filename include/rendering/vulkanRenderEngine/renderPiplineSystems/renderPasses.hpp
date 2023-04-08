#pragma once

#include "core.hpp"
#include "entityPool.hpp"
#include "renderPassesComponents.hpp"

#include <array>
#include <list>
#include <vector>
#include <queue>
#include <string>


namespace vulkanRenderEngine
{
    class renderPasses
    {
        private:
            static inline entityPool *renderPassesPool;
            static inline renderPassesComponents *renderPassesData;

            static void createRenderPass(renderPassInfo& info);
            static void destroyRenderPass(renderPassInfo *info);


        public:
            static void init(entityPool *renderPassesPool);
            static void close();


            static renderPassId create(renderPassInfo& info);
            static void set(renderPassInfo& info);

            static renderPassInfo *get(renderPassId id);
            static void destroy(renderPassId id);
            
    };
}