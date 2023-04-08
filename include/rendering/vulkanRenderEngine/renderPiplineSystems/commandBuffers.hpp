#pragma once

#include "core.hpp"
#include "entityPool.hpp"
#include "commandBuffersComponents.hpp"

#include <array>
#include <list>
#include <vector>
#include <queue>
#include <string>


namespace vulkanRenderEngine
{
    class commandBuffers
    {
         private:
            static inline entityPool *commandBuffersPool;
            static inline commandBuffersComponents *commandBuffersData;

            static inline vk::CommandPool commandPool;

            static void createcommandBuffer(commandBufferInfo& info);
            static void destroycommandBuffer(commandBufferInfo *info);


        public:
            static void init(entityPool *commandBufferesPool);
            static void close();


            static commandBufferId create(commandBufferInfo& info);
            static void set(commandBufferInfo& info);

            static commandBufferInfo *get(commandBufferId id);
            static void destroy(commandBufferId id);
            
    };
}