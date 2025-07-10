#include <Renderer/Shader.hpp>
#include <Renderer/Buffer/VertexBuffer.hpp>
#include <Renderer/Vulkan/Pipeline/GraphicsPipeline.hpp>
#include <Renderer/Vulkan/Shader.hpp>
#include <Renderer/Vulkan/SwapChain.hpp>
#include <Renderer/Vulkan/Buffer/VertexBuffer.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <Renderer/Vulkan/Pipeline/PipelineBuilder.hpp>
#include <array>
#include <stdexcept>
#include "GraphicsPipeline.hpp"

namespace LunaraEngine
{
    GraphicsPipeline::GraphicsPipeline(RendererDataType* rendererData, const ShaderInfo* info,
                                       const std::map<size_t, std::vector<uint32_t>>& shaderSources)
        : Pipeline(rendererData->device)
    {

        PipelineBuilder builder(rendererData, info);
        builder.SetPipelineType(PipelineType::Graphics);
        builder.AddStage(ShaderStage::Vertex, shaderSources.at(VK_SHADER_STAGE_VERTEX_BIT));
        builder.AddStage(ShaderStage::Fragment, shaderSources.at(VK_SHADER_STAGE_FRAGMENT_BIT));
        builder.AddVertexInputInfo();
        builder.SetInputAssembly();
        builder.SetViewportState();
        builder.SetRasterization(PolygonMode::FILL);
        builder.SetSampling();
        builder.AddColorBlending();
        builder.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT);
        builder.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT);

        auto [pipeline, layout, descriptorLayout] = builder.CreatePipeline();

        p_Pipeline = pipeline;
        p_Layout = layout;
        m_DescriptorLayout = descriptorLayout;
    }

    GraphicsPipeline::~GraphicsPipeline()
    {

        if (m_DescriptorLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(p_Device, m_DescriptorLayout, nullptr);
        }
        if (p_Layout != VK_NULL_HANDLE) { vkDestroyPipelineLayout(p_Device, p_Layout, nullptr); }
        if (p_Pipeline != VK_NULL_HANDLE) { vkDestroyPipeline(p_Device, p_Pipeline, nullptr); }
    }


}// namespace LunaraEngine