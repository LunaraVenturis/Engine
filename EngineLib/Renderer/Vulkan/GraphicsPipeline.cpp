#include "GraphicsPipeline.hpp"
#include <array>
#include <stdexcept>

namespace LunaraEngine
{
    GraphicsPipeline::GraphicsPipeline(VkDevice device, const std::vector<uint32_t>& vertexSpriv,
                                       const std::vector<uint32_t>& fragmentSpriv, VkExtent2D viewportSize,
                                       const std::vector<VkDescriptorSetLayout>& layouts, VkRenderPass renderPass)
        : Pipeline(device)
    {
        std::array<VkShaderModule, 2> shaderModules = {};
        shaderModules[0] = CreateShaderModule(vertexSpriv);
        shaderModules[1] = CreateShaderModule(fragmentSpriv);
        CreatePipelineLayout(layouts);
        std::array<VkShaderStageFlagBits, 2> shaderStageFlags = {VK_SHADER_STAGE_VERTEX_BIT,
                                                                 VK_SHADER_STAGE_FRAGMENT_BIT};

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {};

        for (size_t i = 0; i < shaderStages.size(); ++i)
        {
            shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[i].stage = shaderStageFlags[i];
            shaderStages[i].module = shaderModules[i];
            shaderStages[i].pName = "main";
        }
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;// Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;// Optional

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) viewportSize.width;
        viewport.height = (float) viewportSize.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = viewportSize;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;// Optional
        rasterizer.depthBiasClamp = 0.0f;         // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f;   // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;         // Optional
        multisampling.pSampleMask = nullptr;           // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE;// Optional
        multisampling.alphaToOneEnable = VK_FALSE;     // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;// Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;            // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;// Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;            // Optional

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;// Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;// Optional
        colorBlending.blendConstants[1] = 0.0f;// Optional
        colorBlending.blendConstants[2] = 0.0f;// Optional
        colorBlending.blendConstants[3] = 0.0f;// Optional


        std::array<VkDynamicState, 2> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr;// Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = p_Layout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;// Optional
        pipelineInfo.basePipelineIndex = -1;             // Optional

        if (vkCreateGraphicsPipelines(p_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &p_Pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        for (const auto shaderModule: shaderModules) { vkDestroyShaderModule(p_Device, shaderModule, nullptr); }
    }
}// namespace LunaraEngine
