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
            static inline entityPool *commandPoolsPool;
            static inline entityPool *commandBuffersPool;

            static inline commandBuffersComponents *commandBuffersData;


            static void createCommandPool(commandPoolInfo& info);
            static void destroyCommandPool(commandPoolInfo& info);
            
            static void createCommandBuffer(commandBufferInfo& info);
            static void destroyCommandBuffer(commandBufferInfo *info);

        public:
            /**
             * @brief init the commands pools id pool and the data structre to contain all the pools and buffers
             * 
             * @param commandBufferesPool the gloable command buffers pool
             */
            static void init(entityPool *commandBufferesPool);

            /** 
             * @brief   destroy all the command pools and the command buffers, 
             *          all command pools id will be deleted but the command buffers wil remain
             * 
             */
            static void close();

            /**
             * @brief Create a new vulkan command Pool
             * 
             * @param data the spesfaction of the command pool to create
             * @return  the idenifaction of the new command pool in the system
             */
            static commandPoolId createPool(commandPoolInfo& data);

            /**
             * @brief set the data of a vulkan command Pool
             * 
             * @param data the new data of the command pool
             */
            static void setPool(commandPoolInfo& data);

            /**
             * @brief Get the vulkan command Pool data(includes the vulkan object)
             * 
             * @param id the id of the pool to get
             * @return the command pool data
             */
            static commandPoolInfo *getPool(commandBufferId id);

            /**
             * @brief destroy a vulkan command pool and all of it's command buffers and unvalidate  the pool id but not the commandBuffers id's
             * 
             * @param id the pool to destroy
             */
            static void destroyPool(commandBufferId id);

            /**
             * @brief Create a new vulkan command Buffer object
             * 
             * @param data the spesfaction of the command buffer to create
             * @return  the idenifaction of the new command bufferc in the system 
             */
            static commandBufferId createBuffer(commandBufferInfo& data);

            /**
             * @brief set the data of a vulkan command buffer
             * 
             * @param data the new data of the command buffer
             */
            static void setBuffer(commandBufferInfo& data);


            /**
             * @brief Get the vulkan command buffer data(includes the vulkan object)
             * 
             * @param id the id of the buffer to get
             * @return the command buffer data
             */
            static commandBufferInfo *getBuffer(commandBufferId id);

            /**
             * @brief deallocate a vulkan command buffer, won't unvalidate id
             * 
             * @param id the buffer to destroy
             */
            static void destroyBuffer(commandBufferId id);
    };
}