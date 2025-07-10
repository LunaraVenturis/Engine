#include "Pipeline.hpp"
#include "PipelineBuilder.hpp"
#include <stdexcept>

namespace LunaraEngine
{
    Pipeline::Pipeline(VkDevice device) : p_Device(device) {}

    Pipeline::~Pipeline()
    {
        if (p_DescriptorLayouts.size() > 0 && p_DescriptorLayouts[0] != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(p_Device, p_DescriptorLayouts[0], nullptr);
        }
        if (p_Layout != VK_NULL_HANDLE) { vkDestroyPipelineLayout(p_Device, p_Layout, nullptr); }
        if (p_Pipeline != VK_NULL_HANDLE) { vkDestroyPipeline(p_Device, p_Pipeline, nullptr); }
    }

    VkPipeline Pipeline::GetPipeline() const { return p_Pipeline; }

    VkPipelineLayout Pipeline::GetLayout() const { return p_Layout; }

    VkDescriptorSetLayout Pipeline::GetDescriptorLayout() const { return p_DescriptorLayouts[0]; }

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
        p_DescriptorLayouts.push_back(descriptorLayout);
    }

    ComputePipeline::ComputePipeline(RendererDataType* rendererData, const ShaderInfo* info,
                                     const std::map<size_t, std::vector<uint32_t>>& shaderSources)
        : Pipeline(rendererData->device)
    {

        PipelineBuilder builder(rendererData, info);
        builder.SetPipelineType(PipelineType::Compute);
        builder.AddStage(ShaderStage::Compute, shaderSources.at(VK_SHADER_STAGE_COMPUTE_BIT));

        auto [pipeline, layout, descriptorLayout] = builder.CreatePipeline();

        p_Pipeline = pipeline;
        p_Layout = layout;
        p_DescriptorLayouts.push_back(descriptorLayout);
    }
}// namespace LunaraEngine
