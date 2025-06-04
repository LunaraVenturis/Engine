#include <Renderer/Shader.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Renderer/Vulkan/GraphicsPipeline.hpp>
#include <Renderer/Vulkan/Shader.hpp>
#include <Renderer/Vulkan/SwapChain.hpp>
#include <Renderer/Vulkan/VertexBuffer.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <array>
#include <stdexcept>

namespace LunaraEngine
{
    GraphicsPipeline::GraphicsPipeline(RendererDataType* rendererData, const ShaderInfo* info,
                                       const std::map<size_t, std::vector<uint32_t>>& shaderSources)
        : Pipeline(rendererData->device)
    {
        CreateDescriptorSets(rendererData, *info);

        CreatePipelineLayout(m_DescriptorSets);

        std::array<VkShaderModule, 2> shaderModules = {};
        std::array<VkShaderStageFlagBits, 2> shaderStageFlags = {VK_SHADER_STAGE_VERTEX_BIT,
                                                                 VK_SHADER_STAGE_FRAGMENT_BIT};
        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {};

        for (size_t i = 0; i < shaderStages.size(); ++i)
        {
            const std::vector<uint32_t>& shaderSource = shaderSources.at(shaderStageFlags[i]);
            shaderModules[i] = CreateShaderModule(shaderSource);
            shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[i].stage = shaderStageFlags[i];
            shaderStages[i].module = shaderModules[i];
            shaderStages[i].pName = "main";
        }

        VkVertexInputBindingDescription bindingDescription;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo =
                CreateVertexInputInfo(*info, bindingDescription, attributeDescriptions);

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 0;
        viewportState.pViewports = nullptr;
        viewportState.scissorCount = 0;
        viewportState.pScissors = nullptr;

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


        std::array<VkDynamicState, 2> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
                                                       VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT};
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
        pipelineInfo.renderPass = rendererData->swapChain->GetRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;// Optional
        pipelineInfo.basePipelineIndex = -1;             // Optional

        if (vkCreateGraphicsPipelines(p_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &p_Pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        for (const auto shaderModule: shaderModules) { vkDestroyShaderModule(p_Device, shaderModule, nullptr); }
    }

    GraphicsPipeline::~GraphicsPipeline()
    {

        if (p_Layout != VK_NULL_HANDLE) { vkDestroyPipelineLayout(p_Device, p_Layout, nullptr); }
        if (p_Pipeline != VK_NULL_HANDLE) { vkDestroyPipeline(p_Device, p_Pipeline, nullptr); }
        if (m_DescriptorSets.size() > 0)
        {
            for (const auto& descriptorSet: m_DescriptorSets)
            {
                vkDestroyDescriptorSetLayout(p_Device, descriptorSet, nullptr);
            }
            m_DescriptorSets.clear();
        }
    }

    VkDescriptorType GraphicsPipeline::GetDescriptorType(ShaderResourceType type)
    {
        switch (type)
        {
            case ShaderResourceType::Texture:
                return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            case ShaderResourceType::Sampler:
                return VK_DESCRIPTOR_TYPE_SAMPLER;
            case ShaderResourceType::UniformBuffer:
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case ShaderResourceType::StorageBuffer:
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            default:
                return VK_DESCRIPTOR_TYPE_MAX_ENUM;
        }
    }

    void GraphicsPipeline::CreateDescriptorSets(RendererDataType* rendererData, const ShaderInfo& info)
    {
        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
        for (const auto& resource: info.resources.bufferResources)
        {
            const auto& type = resource.type;
            const auto& name = resource.name;
            size_t size = resource.size;
            ShaderBindingT binding = resource.layout.binding;
            ShaderResourceMemoryLayout layout = resource.layout.layoutType;

            //If memory model layout is missing put default
            if (resource.layout.layoutType == ShaderResourceMemoryLayout::None)
            {
                layout = ShaderResourceMemoryLayout::STD140;
            }

            (void) name;
            (void) size;
            (void) layout;

            VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
            descriptorSetLayoutBinding.binding = binding;
            descriptorSetLayoutBinding.descriptorType = GraphicsPipeline::GetDescriptorType(type);

            //For now resources are availeble in all stages
            descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;

            //For now only one resource per binding
            descriptorSetLayoutBinding.descriptorCount = 1;

            //For now no immutable samplers
            descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

            descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
        layoutInfo.pBindings = descriptorSetLayoutBindings.data();

        //if there are no bindings exit
        if (layoutInfo.bindingCount == 0) return;

        VkDescriptorSetLayout layout;
        if (vkCreateDescriptorSetLayout(rendererData->device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        m_DescriptorSets.push_back(layout);
    }

    VkPipelineVertexInputStateCreateInfo
    GraphicsPipeline::CreateVertexInputInfo(const ShaderInfo& info, VkVertexInputBindingDescription& bindingDescription,
                                            std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
    {
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;// Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;// Optional

        size_t stride = 0;
        for (const ShaderInputResource& resource: info.resources.inputResources)
        {
            stride += Shader::GetInputResourceSize(resource);
        }

        bindingDescription.binding = info.resources.inputResources[0].binding;
        bindingDescription.stride = (uint32_t) stride;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        for (const ShaderInputResource& resource: info.resources.inputResources)
        {
            VkVertexInputAttributeDescription description{};
            description.binding = resource.binding;
            description.location = resource.location;
            description.format = VulkanShader::GetShaderResourceFormat(resource.format, resource.type);
            description.offset = resource.offset;

            attributeDescriptions.push_back(description);
        }

        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());

        return vertexInputInfo;
    }

}// namespace LunaraEngine
