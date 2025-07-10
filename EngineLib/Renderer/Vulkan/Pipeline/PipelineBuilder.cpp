#include "PipelineBuilder.hpp"
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <Renderer/Vulkan/Shader.hpp>

namespace LunaraEngine
{
    PipelineBuilder::PipelineBuilder(RendererDataType* rendererData, const ShaderInfo* info)
        : m_RendererData(rendererData), m_Info(info)
    {}

    void PipelineBuilder::SetPipelineType(PipelineType type) { m_PipelineType = type; }

    void PipelineBuilder::AddDescriptorSetLayout(VkDescriptorSetLayout layout) { m_DescriptorLayout = layout; }

    void PipelineBuilder::CreateDescriptorSetLayout()
    {
        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
        for (const auto& resource: m_Info->resources.bufferResources)
        {
            if (resource.type == BufferResourceType::PushConstant) { continue; }

            const auto& type = resource.type;
            const auto& name = resource.name;
            ShaderBinding binding = resource.layout.binding;
            BufferResourceMemoryLayout layout = resource.layout.layoutType;

            if (resource.layout.layoutType == BufferResourceMemoryLayout::None)
            {
                layout = BufferResourceMemoryLayout::STD140;
            }

            VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
            descriptorSetLayoutBinding.binding = (uint32_t) binding;
            descriptorSetLayoutBinding.descriptorType = GetDescriptorType(type);
            descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            descriptorSetLayoutBinding.descriptorCount = 1;
            descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
            descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

            (void) name;
            (void) layout;
        }
        for (const auto& resource: m_Info->resources.textureResources)
        {
            const auto& type = resource.type;
            const auto& name = resource.name;
            ShaderBinding binding = resource.layout.binding;

            VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
            descriptorSetLayoutBinding.binding = (uint32_t) binding;
            descriptorSetLayoutBinding.descriptorType = GetDescriptorType(type);
            descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            descriptorSetLayoutBinding.descriptorCount = 1;
            descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
            descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);
            (void) name;
        }
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
        layoutInfo.pBindings = descriptorSetLayoutBindings.data();

        //if there are no bindings exit
        if (layoutInfo.bindingCount == 0) return;

        if (vkCreateDescriptorSetLayout(m_RendererData->device, &layoutInfo, nullptr, &m_DescriptorLayout) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void PipelineBuilder::CreatePushConstantRanges()
    {
        for (auto& resource: m_Info->resources.bufferResources)
        {
            if (resource.type == BufferResourceType::PushConstant)
            {
                VkPushConstantRange range{};
                range.offset = 0;
                range.size = (uint32_t) resource.length * (uint32_t) resource.stride;
                range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                m_ConstantRanges.push_back(range);
            }
        }
    }

    void PipelineBuilder::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_DescriptorLayout;

        if (m_ConstantRanges.empty())
        {
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;
        }
        else
        {
            pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(m_ConstantRanges.size());
            pipelineLayoutInfo.pPushConstantRanges = m_ConstantRanges.data();
        }
        if (vkCreatePipelineLayout(m_RendererData->device, &pipelineLayoutInfo, nullptr, &m_Layout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    VkShaderModule PipelineBuilder::CreateShaderModule(const std::vector<uint32_t>& spirvCode) const
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = spirvCode.size();
        createInfo.pCode = spirvCode.data();
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_RendererData->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }
        return shaderModule;
    }

    void PipelineBuilder::AddStage(ShaderStage stage, const std::vector<uint32_t>& spirvCode)
    {
        VkShaderStageFlagBits flagBits;
        switch (stage)
        {
            case ShaderStage::Vertex:
                flagBits = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case ShaderStage::Fragment:
                flagBits = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case ShaderStage::Compute:
                flagBits = VK_SHADER_STAGE_COMPUTE_BIT;
                break;
        }

        m_ShaderModules.push_back(CreateShaderModule(spirvCode));
        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage = flagBits;
        stageInfo.module = m_ShaderModules[m_ShaderModules.size() - 1];
        stageInfo.pName = "main";
        m_ShaderStages.push_back(stageInfo);
    }

    void PipelineBuilder::AddVertexInputInfo()
    {
        m_VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        m_VertexInputInfo.vertexBindingDescriptionCount = 0;
        m_VertexInputInfo.pVertexBindingDescriptions = nullptr;// Optional
        m_VertexInputInfo.vertexAttributeDescriptionCount = 0;
        m_VertexInputInfo.pVertexAttributeDescriptions = nullptr;// Optional

        size_t stride = 0;
        for (const ShaderInputResource& resource: m_Info->resources.inputResources)
        {
            stride += Shader::GetInputResourceSize(resource);
        }

        if (stride > 0)
        {
            m_BindingDescription.binding = (uint32_t) m_Info->resources.inputResources[0].binding;
            m_BindingDescription.stride = (uint32_t) stride;
            m_BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            m_VertexInputInfo.pVertexBindingDescriptions = &m_BindingDescription;
            m_VertexInputInfo.vertexBindingDescriptionCount = 1;
        }

        for (const ShaderInputResource& resource: m_Info->resources.inputResources)
        {
            VkVertexInputAttributeDescription description{};
            description.binding = (uint32_t) resource.binding;
            description.location = resource.location;
            description.format = VulkanShader::GetBufferResourceFormat(resource.format, resource.type);
            description.offset = resource.offset;

            m_AttributeDescriptions.push_back(description);
        }
        if (m_AttributeDescriptions.size() > 0)
        {
            m_VertexInputInfo.pVertexAttributeDescriptions = m_AttributeDescriptions.data();
            m_VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());
        }
    }

    void PipelineBuilder::SetInputAssembly(RenderingBasePrimitive primitive)
    {
        m_InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        m_InputAssembly.topology = GetPrimitiveTopology(primitive);
        m_InputAssembly.primitiveRestartEnable = VK_FALSE;
    }

    void PipelineBuilder::SetViewportState(std::vector<VkViewport> viewports, std::vector<VkRect2D> scissors)
    {
        m_ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        if (viewports.size() > 0)
        {
            m_ViewportState.viewportCount = static_cast<uint32_t>(viewports.size());
            m_ViewportState.pViewports = viewports.data();
        }
        if (scissors.size() > 0)
        {
            m_ViewportState.scissorCount = static_cast<uint32_t>(scissors.size());
            m_ViewportState.pScissors = scissors.data();
        }
    }

    void PipelineBuilder::SetRasterization(PolygonMode mode)
    {
        m_Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        m_Rasterizer.depthClampEnable = VK_FALSE;
        m_Rasterizer.rasterizerDiscardEnable = VK_FALSE;
        m_Rasterizer.polygonMode = GetPolygonMode(mode);
        m_Rasterizer.lineWidth = 1.0f;
        m_Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        m_Rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        m_Rasterizer.depthBiasEnable = VK_FALSE;
        m_Rasterizer.depthBiasConstantFactor = 0.0f;// Optional
        m_Rasterizer.depthBiasClamp = 0.0f;         // Optional
        m_Rasterizer.depthBiasSlopeFactor = 0.0f;   // Optional
    }

    void PipelineBuilder::SetSampling()
    {
        m_Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        m_Multisampling.sampleShadingEnable = VK_FALSE;
        m_Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        m_Multisampling.minSampleShading = 1.0f;         // Optional
        m_Multisampling.pSampleMask = nullptr;           // Optional
        m_Multisampling.alphaToCoverageEnable = VK_FALSE;// Optional
        m_Multisampling.alphaToOneEnable = VK_FALSE;     // Optional
    }

    void PipelineBuilder::AddColorBlending()
    {

        m_BlendAttachments.push_back({});

        VkPipelineColorBlendAttachmentState& colorBlendAttachment = m_BlendAttachments.back();
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;          // Multiply by fragment alpha
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;// Inverse of it
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;// FinalColor = src + dst * (1 - src.a)

        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;// For alpha channel itself
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        m_ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        m_ColorBlending.logicOpEnable = VK_FALSE;
        m_ColorBlending.logicOp = VK_LOGIC_OP_COPY;// Optional
        m_ColorBlending.attachmentCount = 1;
        m_ColorBlending.pAttachments = &colorBlendAttachment;
        m_ColorBlending.blendConstants[0] = 0.0f;// Optional
        m_ColorBlending.blendConstants[1] = 0.0f;// Optional
        m_ColorBlending.blendConstants[2] = 0.0f;// Optional
        m_ColorBlending.blendConstants[3] = 0.0f;// Optional
    }

    void PipelineBuilder::AddDynamicState(VkDynamicState state)
    {
        m_DynamicStates.push_back(state);
        m_DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        m_DynamicState.dynamicStateCount = static_cast<uint32_t>(m_DynamicStates.size());
        m_DynamicState.pDynamicStates = m_DynamicStates.data();
    }

    std::tuple<VkPipeline, VkPipelineLayout, VkDescriptorSetLayout> PipelineBuilder::CreatePipeline()
    {
        CreateDescriptorSetLayout();
        CreatePushConstantRanges();
        CreatePipelineLayout();

        VkPipeline pipeline;

        switch (m_PipelineType)
        {
            case PipelineType::Graphics: {
                VkGraphicsPipelineCreateInfo pipelineInfo{};
                pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                pipelineInfo.stageCount = static_cast<uint32_t>(m_ShaderStages.size());
                pipelineInfo.pStages = m_ShaderStages.data();

                pipelineInfo.pVertexInputState = &m_VertexInputInfo;
                pipelineInfo.pInputAssemblyState = &m_InputAssembly;
                pipelineInfo.pViewportState = &m_ViewportState;
                pipelineInfo.pRasterizationState = &m_Rasterizer;
                pipelineInfo.pMultisampleState = &m_Multisampling;
                pipelineInfo.pDepthStencilState = nullptr;
                pipelineInfo.pColorBlendState = &m_ColorBlending;
                pipelineInfo.pDynamicState = &m_DynamicState;
                pipelineInfo.layout = m_Layout;
                pipelineInfo.renderPass = m_RendererData->swapChain->GetRenderPass();
                pipelineInfo.subpass = 0;
                pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;// Optional
                pipelineInfo.basePipelineIndex = -1;             // Optional

                if (vkCreateGraphicsPipelines(
                            m_RendererData->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
                {
                    throw std::runtime_error("failed to create graphics pipeline!");
                }
                break;
            }
            case PipelineType::Compute: {
                VkComputePipelineCreateInfo pipelineInfo{};
                pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
                pipelineInfo.stage = m_ShaderStages[0];
                pipelineInfo.layout = m_Layout;
                pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;// Optional
                pipelineInfo.basePipelineIndex = -1;             // Optional

                if (vkCreateComputePipelines(
                            m_RendererData->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
                {
                    throw std::runtime_error("failed to create graphics pipeline!");
                }
            }
            break;
        }

        for (const auto shaderModule: m_ShaderModules)
        {
            vkDestroyShaderModule(m_RendererData->device, shaderModule, nullptr);
        }
        return {pipeline, m_Layout, m_DescriptorLayout};
    }

    VkPrimitiveTopology PipelineBuilder::GetPrimitiveTopology(RenderingBasePrimitive primitive) const
    {
        switch (primitive)
        {
            case RenderingBasePrimitive::POINTS:
                return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                break;
            case RenderingBasePrimitive::LINES:
                return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                break;
            case RenderingBasePrimitive::LINE_STRIP:
                return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
                break;
            case RenderingBasePrimitive::TRIANGLES:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                break;
            case RenderingBasePrimitive::TRIANGLE_STRIP:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
                break;
            case RenderingBasePrimitive::TRIANGLE_FAN:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
                break;
            case RenderingBasePrimitive::LINES_WITH_ADJACENCY:
                return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
                break;
            case RenderingBasePrimitive::LINE_STRIP_WITH_ADJACENCY:
                return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
                break;
            case RenderingBasePrimitive::TRIANGLES_WITH_ADJACENCY:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
                break;
            case RenderingBasePrimitive::TRIANGLE_STRIP_WITH_ADJACENCY:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
                break;
            case RenderingBasePrimitive::PATCHES:
                return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
                break;
            case RenderingBasePrimitive::None:
            default:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                break;
        }
    }

    VkPolygonMode PipelineBuilder::GetPolygonMode(PolygonMode mode) const
    {
        switch (mode)
        {
            case PolygonMode::FILL:
                return VK_POLYGON_MODE_FILL;
            case PolygonMode::LINE:
                return VK_POLYGON_MODE_LINE;
            case PolygonMode::POINT:
                return VK_POLYGON_MODE_POINT;
            case PolygonMode::None:
            default:
                return VK_POLYGON_MODE_FILL;
        }
        return {};
    }

    VkDescriptorType PipelineBuilder::GetDescriptorType(BufferResourceType type)
    {
        switch (type)
        {
            case BufferResourceType::Texture:
            case BufferResourceType::Texture2D:
            case BufferResourceType::Texture2DArray:
            case BufferResourceType::Texture3D:
                return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

            case BufferResourceType::UniformBuffer:
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            case BufferResourceType::StorageBuffer:
            case BufferResourceType::Buffer:
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

            case BufferResourceType::PushConstant:
            case BufferResourceType::None:
            default:
                throw std::runtime_error("Invalid or unsupported BufferResourceType");
        }
    }


}// namespace LunaraEngine