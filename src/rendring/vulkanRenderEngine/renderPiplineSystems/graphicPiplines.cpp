#include "vulkanRenderEngine/renderPiplineSystems/graphicPiplines.hpp"
#include "vulkanRenderEngine/renderPiplineSystems/renderPasses.hpp"
#include "vulkanRenderEngine/renderPiplineSystems/vertexArrayManger.hpp"
#include "device.hpp"
#include "log.hpp"

#include <fstream>
#include <vulkan/vk_enum_string_helper.h>

namespace vulkanRenderEngine
{
    void graphicPiplines::init(entityPool *shadersPool)
    {
        graphicPiplines::shadersPool = shadersPool;
        graphicsPipelinesInfo = new graphicsPipelineComponents(shadersPool);
    }

    void graphicPiplines::close()
    {
        for (int i = 0; i < graphicsPipelinesInfo->getData().size(); i++)
            destroyGraphicsPipeline(&graphicsPipelinesInfo->getData()[i]);

        delete graphicsPipelinesInfo;
    }

    shaderId graphicPiplines::create(graphicsPipelineInfo &data)
    {
        shaderId id = shadersPool->allocEntity();
        data.id = id;
        recreateGraphicsPipeline(data);
        graphicsPipelinesInfo->setComponent(id, data);
        return id;
    }

    void graphicPiplines::set(graphicsPipelineInfo &data)
    {
        recreateGraphicsPipeline(data);
        graphicsPipelinesInfo->setComponent(data.id, data);
    }

    void graphicPiplines::destroy(shaderId id)
    {
        destroyGraphicsPipeline(graphicsPipelinesInfo->getComponent(id));

        graphicsPipelinesInfo->deleteComponent(id);
    }

    graphicsPipelineInfo *graphicPiplines::get(shaderId id)
    {
        return graphicsPipelinesInfo->getComponent(id);
    }

    void graphicPiplines::recreateGraphicsPipeline(graphicsPipelineInfo &toRecreate)
    {
        vk::RenderPass compatibleRenderPass = renderPasses::get(toRecreate.renderPassId)->vkObject;

        auto fragShaderCode = readFile(toRecreate.fragShaderCodePath);
        auto vertShaderCode = readFile(toRecreate.vertShaderCodePath);

        vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
        vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        std::vector<vk::DynamicState> dynamicStates{
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor};

        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.vertexBindingDescriptionCount = vaos::getBindingDescriptionsSize(toRecreate.vao);
        vertexInputInfo.pVertexBindingDescriptions = vaos::getBindingDescriptions(toRecreate.vao);
        vertexInputInfo.vertexAttributeDescriptionCount = vaos::getAttributeDescriptionsSize(toRecreate.vao);
        vertexInputInfo.pVertexAttributeDescriptions = vaos::getAttributeDescriptions(toRecreate.vao);

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eNone;
        rasterizer.frontFace = vk::FrontFace::eClockwise;

        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
        colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
        colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        createDescriptorSetLayout(toRecreate);
        createDescriptorSetsPool(toRecreate);
        createDescriptorSets(toRecreate);

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &toRecreate.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        try
        {
            toRecreate.pipelineLayout = device::getDevice().createPipelineLayout(pipelineLayoutInfo);
        }
        catch (vk::SystemError err)
        {
            LOG_FATAL("failed to create pipeline layout! " << err.what());
        }

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = toRecreate.pipelineLayout;
        pipelineInfo.renderPass = compatibleRenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        try
        {
            toRecreate.graphicsPipeline = device::getDevice().createGraphicsPipeline(nullptr, pipelineInfo).value;
        }
        catch (vk::SystemError err)
        {
            LOG_FATAL("failed to create pipeline! " << err.what());
        }

        device::getDevice().destroyShaderModule(fragShaderModule);
        device::getDevice().destroyShaderModule(vertShaderModule);

        delete[] vertexInputInfo.pVertexBindingDescriptions;
        delete[] vertexInputInfo.pVertexAttributeDescriptions;
    }

    std::vector<char> graphicPiplines::readFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }

    vk::ShaderModule graphicPiplines::createShaderModule(const std::vector<char> &code)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.flags = vk::ShaderModuleCreateFlags();
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

        try
        {
            return device::getDevice().createShaderModule(createInfo);
        }
        catch (vk::SystemError err)
        {
            LOG_FATAL("Failed to create shader module: " << err.what());
        }
    }

    void graphicPiplines::createDescriptorSetLayout(graphicsPipelineInfo &toRecreate)
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;
        toRecreate.descriptorSetLayout = device::getDevice().createDescriptorSetLayout(layoutInfo, nullptr);
    }

    void graphicPiplines::createDescriptorSetsPool(graphicsPipelineInfo &toRecreate)
    {
        vk::DescriptorPoolSize poolSize{};
        poolSize.type = vk::DescriptorType::eUniformBuffer;
        poolSize.descriptorCount = toRecreate.descriptorPoolSize;

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = toRecreate.descriptorPoolSize;

        toRecreate.descriptorPool = device::getDevice().createDescriptorPool(poolInfo);
    }

    void graphicPiplines::createDescriptorSets(graphicsPipelineInfo &toRecreate)
    {
        std::vector<vk::DescriptorSetLayout> layouts(toRecreate.descriptorPoolSize, toRecreate.descriptorSetLayout);
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = toRecreate.descriptorPool;
        allocInfo.descriptorSetCount = toRecreate.descriptorPoolSize;
        allocInfo.pSetLayouts = layouts.data();

        *toRecreate.descriptorSets = device::getDevice().allocateDescriptorSets(allocInfo);

        // for (size_t i = 0; i < toRecreate.descriptorPoolSize; i++)
        // {
        //     vk::DescriptorBufferInfo bufferInfo{};
        //     bufferInfo.buffer = uniformBuffers[i];
        //     bufferInfo.offset = 0;
        //     bufferInfo.range = sizeof(UniformBufferObject);

        //     vk::WriteDescriptorSet descriptorWrite{};
        //     descriptorWrite.sType = vk::StructureType::eWriteDescriptorSet;
        //     descriptorWrite.dstSet = toRecreate->descriptorSets[i];
        //     descriptorWrite.dstBinding = 0;
        //     descriptorWrite.dstArrayElement = 0;
        //     descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
        //     descriptorWrite.descriptorCount = 1;
        //     descriptorWrite.pBufferInfo = &bufferInfo;

        //     vk::UpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
        // }
    }

    void graphicPiplines::destroyGraphicsPipeline(graphicsPipelineInfo *toDestroy)
    {
        device::getDevice().destroyDescriptorPool(toDestroy->descriptorPool);
        device::getDevice().destroyDescriptorSetLayout(toDestroy->descriptorSetLayout);
        device::getDevice().destroyPipelineLayout(toDestroy->pipelineLayout);
        device::getDevice().destroyPipeline(toDestroy->graphicsPipeline);
    }
}