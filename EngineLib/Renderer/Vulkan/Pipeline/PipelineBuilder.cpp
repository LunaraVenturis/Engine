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
        m_ShaderStages.push_back({});
        m_ShaderStages.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStages.back().stage = flagBits;
        m_ShaderStages.back().module = m_ShaderModules.back();
        m_ShaderStages.back().pName = "main";
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
            description.format = VulkanShader::GetShaderResourceFormat(resource.format, resource.type);
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

    void PipelineBuilder::AddColorBlending(VkPipelineColorBlendStateCreateInfo colorBlending)
    {
        // m_ColorBlendAttachments.push_back(colorBlendAttachment);
        m_ColorBlending = colorBlending;
        for (uint32_t i = 0; i < colorBlending.attachmentCount; i++)
        {
            m_BlendAttachments.push_back(colorBlending.pAttachments[i]);
        }
        m_ColorBlending.pAttachments = m_BlendAttachments.data();
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
        }
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
        }
        return {};
    }
}// namespace LunaraEngine