#pragma once

#include "core.hpp"
#include "entityPool.hpp"
#include "graphicsPipelineComponents.hpp"

#include <array>
#include <list>
#include <vector>
#include <queue>
#include <string>

namespace vulkanRenderEngine
{
    class graphicPiplines
    {
    private:
        static inline entityPool *shadersPool;
        static inline graphicsPipelineComponents *graphicsPipelinesInfo;

        static void recreateGraphicsPipeline(graphicsPipelineInfo &toRecreate);

        static std::vector<char> readFile(const std::string &filename);
        static vk::ShaderModule createShaderModule(const std::vector<char> &code);
        static void createDescriptorSetLayout(graphicsPipelineInfo &toRecreate);
        static void createDescriptorSetsPool(graphicsPipelineInfo &toRecreate);
        static void createDescriptorSets(graphicsPipelineInfo &toRecreate);

        static void destroyGraphicsPipeline(graphicsPipelineInfo *toDestroy);

    public:
        static void init(entityPool *shadersPool);
        static void close();

        static shaderId create(graphicsPipelineInfo &info);
        static void set(graphicsPipelineInfo &info);
        static void destroy(shaderId id);

        static graphicsPipelineInfo *get(shaderId id);
    };
}