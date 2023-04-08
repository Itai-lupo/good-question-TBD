#pragma once 

#include "core.hpp"
#include "entityPool.hpp"

#include <array>
#include <string>
#include <vulkan/vulkan.hpp>

struct graphicsPipelineInfo
{
    vkSurfaceId id;
    
    renderPassId renderPassId;
    std::string fragShaderCodePath;
    std::string vertShaderCodePath;

    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;
    
};


class graphicsPipelineComponents 
{
    private:
        entityPool *pool;

        std::vector<graphicsPipelineInfo> data;

        uint32_t *IdToIndex;  
        std::vector<entityId> indexToId;  
        

        static void deleteCallback(void * data, entityId id)
        {
            graphicsPipelineComponents  *This = static_cast<graphicsPipelineComponents *>(data);
            This->deleteComponent(id);
        }

    public:
        graphicsPipelineComponents (entityPool *pool);
        ~graphicsPipelineComponents ();

        void deleteComponent(entityId id);
        graphicsPipelineInfo *getComponent(entityId id);
        void setComponent(entityId id, graphicsPipelineInfo& buffer);
        std::vector<graphicsPipelineInfo>& getData()
        {
            return data;
        }
};
