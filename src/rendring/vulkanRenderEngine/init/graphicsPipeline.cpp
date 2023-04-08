// #include "graphicsPipeline.hpp"
// #include "device.hpp"
// #include "log.hpp"
// #include "swapchain.hpp"

// #include <fstream>

// namespace vulkanRenderEngine
// {


//     void graphicsPipeline::init(entityPool *surfacesPool)
//     {
//         graphicsPipelinesInfo = new graphicsPipelineComponents(surfacesPool);
//     }

//     void graphicsPipeline::close()
//     {
//         for(int i = 0; i < graphicsPipelinesInfo->getData().size(); i++)
//             destroyGraphicsPipeline(&graphicsPipelinesInfo->getData()[i]);

//         delete graphicsPipelinesInfo;
//     }


//     void graphicsPipeline::create(vkSurfaceId id)
//     {
//         graphicsPipelineInfo temp;

//         vk::RenderPass compatibleRenderPass = renderPasses::get(compatibleRenderPassId).vkObject;
        
//         auto fragShaderCode = readFile("assets/shaders/fragment/vulkanPipline.frag.spv");
//         auto vertShaderCode = readFile("assets/shaders/vertex/vulkanPipline.vert.spv");


//         vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
//         vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

//         vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
//         vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
//         vertShaderStageInfo.module = vertShaderModule;
//         vertShaderStageInfo.pName = "main";

//         vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
//         fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
//         fragShaderStageInfo.module = fragShaderModule;
//         fragShaderStageInfo.pName = "main";
        
//         vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        
//         std::vector<vk::DynamicState> dynamicStates{
//             vk::DynamicState::eViewport,
//             vk::DynamicState::eScissor
//         };

//         vk::PipelineDynamicStateCreateInfo dynamicState{};
//         dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
//         dynamicState.pDynamicStates = dynamicStates.data();

//         vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
//         vertexInputInfo.vertexBindingDescriptionCount = 0;
//         vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
//         vertexInputInfo.vertexAttributeDescriptionCount = 0;
//         vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

//         vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
//         inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
//         inputAssembly.primitiveRestartEnable = VK_FALSE;

//         // static specifiecation
//         // VkViewport viewport{};
//         // viewport.x = 0.0f;
//         // viewport.y = 0.0f;
//         // viewport.width = (float) swapchain::getSwapChain(id)->extent.width;
//         // viewport.height = (float) swapchain::getSwapChain(id)->extent.height;
//         // viewport.minDepth = 0.0f;
//         // viewport.maxDepth = 1.0f;
        
//         // VkRect2D scissor{};
//         // scissor.offset = {0, 0};
//         // scissor.extent = swapchain::getSwapChain(id)->extent;

//         // vk::PipelineViewportStateCreateInfo viewportState{};\
//         // viewportState.viewportCount = 1;
//         // viewportState.pViewports = &viewport;
//         // viewportState.scissorCount = 1;
//         // viewportState.pScissors = &scissor;


//         vk::PipelineViewportStateCreateInfo viewportState{};
//         viewportState.viewportCount = 1;
//         viewportState.scissorCount = 1;

//         vk::PipelineRasterizationStateCreateInfo rasterizer{};
//         rasterizer.depthClampEnable =  VK_FALSE;
//         rasterizer.rasterizerDiscardEnable = VK_FALSE;
//         rasterizer.polygonMode = vk::PolygonMode::eFill;
//         rasterizer.lineWidth = 1.0f;
//         rasterizer.cullMode = vk::CullModeFlagBits::eBack;
//         rasterizer.frontFace = vk::FrontFace::eClockwise;

//         vk::PipelineMultisampleStateCreateInfo multisampling{};
//         multisampling.sampleShadingEnable = VK_FALSE;
//         multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
//         multisampling.minSampleShading = 1.0f; // Optional
//         multisampling.pSampleMask = nullptr; // Optional
//         multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
//         multisampling.alphaToOneEnable = VK_FALSE; // Optional

//         vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
//         colorBlendAttachment.colorWriteMask =  vk::ColorComponentFlagBits::eR |  vk::ColorComponentFlagBits::eG |  vk::ColorComponentFlagBits::eB |  vk::ColorComponentFlagBits::eA;
//         colorBlendAttachment.blendEnable = VK_FALSE;
//         colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;  // Optional
//         colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha; // Optional
//         colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
//         colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
//         colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
//         colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional

//         vk::PipelineColorBlendStateCreateInfo colorBlending{};
//         colorBlending.logicOpEnable = VK_FALSE;
//         colorBlending.logicOp = vk::LogicOp::eCopy; // Optional
//         colorBlending.attachmentCount = 1;
//         colorBlending.pAttachments = &colorBlendAttachment;
//         colorBlending.blendConstants[0] = 0.0f; // Optional
//         colorBlending.blendConstants[1] = 0.0f; // Optional
//         colorBlending.blendConstants[2] = 0.0f; // Optional
//         colorBlending.blendConstants[3] = 0.0f; // Optional
        
        

//         vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
//         pipelineLayoutInfo.setLayoutCount = 0; // Optional
//         pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
//         pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
//         pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
        
//         try
//         {
//             temp.pipelineLayout = device::getDevice().createPipelineLayout(pipelineLayoutInfo);
//         }
//         catch(vk::SystemError err)
//         {
//             LOG_FATAL("failed to create pipeline layout! " << err.what());
//         }

//         vk::GraphicsPipelineCreateInfo pipelineInfo{};
//         pipelineInfo.stageCount = 2;
//         pipelineInfo.pStages = shaderStages;
//         pipelineInfo.pVertexInputState = &vertexInputInfo;
//         pipelineInfo.pInputAssemblyState = &inputAssembly;
//         pipelineInfo.pViewportState = &viewportState;
//         pipelineInfo.pRasterizationState = &rasterizer;
//         pipelineInfo.pMultisampleState = &multisampling;
//         pipelineInfo.pDepthStencilState = nullptr; // Optional
//         pipelineInfo.pColorBlendState = &colorBlending;
//         pipelineInfo.pDynamicState = &dynamicState;
//         pipelineInfo.layout = temp.pipelineLayout;
//         pipelineInfo.renderPass = compatibleRenderPass;
//         pipelineInfo.subpass = 0;
//         pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
//         pipelineInfo.basePipelineIndex = -1; // Optional
 
//         try
//         {
//             temp.graphicsPipeline = device::getDevice().createGraphicsPipeline(nullptr, pipelineInfo).value;
//         }
//         catch(vk::SystemError err)
//         {
//             LOG_FATAL("failed to create pipeline! " << err.what());
//         }

//         device::getDevice().destroyShaderModule(fragShaderModule);
//         device::getDevice().destroyShaderModule(vertShaderModule);
        
//         graphicsPipelinesInfo->setComponent(id, temp);
//     }

//     void graphicsPipeline::destroy(vkSurfaceId id)
//     {
//         destroyGraphicsPipeline(graphicsPipelinesInfo->getComponent(id));
//         graphicsPipelinesInfo->deleteComponent(id);
//     }

    
//     graphicsPipelineInfo *graphicsPipeline::get(vkSurfaceId id)
//     {
//         return graphicsPipelinesInfo->getComponent(id);
//     }

//     std::vector<char> graphicsPipeline::readFile(const std::string& filename) {
//         std::ifstream file(filename, std::ios::ate | std::ios::binary);

//         if (!file.is_open()) {
//             throw std::runtime_error("failed to open file!");
//         }

//         size_t fileSize = (size_t) file.tellg();
//         std::vector<char> buffer(fileSize);
//         file.seekg(0);
//         file.read(buffer.data(), fileSize);
//         file.close();
//         return buffer;
//     }

//     vk::ShaderModule graphicsPipeline::createShaderModule(const std::vector<char>& code) {
//         vk::ShaderModuleCreateInfo createInfo{};
// 		createInfo.flags = vk::ShaderModuleCreateFlags();
//         createInfo.codeSize = code.size();
//         createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        
//         try {
// 			return device::getDevice().createShaderModule(createInfo);
// 		}
// 		catch (vk::SystemError err) {
//             LOG_FATAL("Failed to create shader module: " << err.what());
// 		}
        
//     }

    

//     void graphicsPipeline::destroyGraphicsPipeline(graphicsPipelineInfo *toDestroy)
//     {
//         device::getDevice().destroyPipelineLayout(toDestroy->pipelineLayout);   
//         device::getDevice().destroyPipeline(toDestroy->graphicsPipeline);   
//     }



// }