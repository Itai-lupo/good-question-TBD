#pragma once

#include <vulkan/vulkan.hpp>

#include "core.hpp"
#include "graphicsPipelineComponents.hpp"

namespace vulkanRenderEngine
{
    class graphicsPipeline
    {
        private:
            static inline graphicsPipelineComponents *graphicsPipelinesInfo;
            
            static std::vector<char> readFile(const std::string& filename);
            static vk::ShaderModule createShaderModule(const std::vector<char>& code);
            
            static void destroyGraphicsPipeline(graphicsPipelineInfo *toDestroy);
            static vk::RenderPass createRenderPass(vkSurfaceId id);

        public:
            static void init(entityPool *surfacesPool);
            static void close();


            static void create(vkSurfaceId id);
            static void destroy(vkSurfaceId id);

            static graphicsPipelineInfo *get(vkSurfaceId id);
    };
    
} 