#include "Pipeline.hpp"
#include "PipelineBuilder.hpp"
#include <stdexcept>

namespace LunaraEngine
{
    Pipeline::Pipeline(VkDevice device) : p_Device(device) {}

    Pipeline::~Pipeline()
    {
        for (auto& [index, layout]: p_DescriptorLayouts)
        {
            (void) index;
            if (layout != VK_NULL_HANDLE) { vkDestroyDescriptorSetLayout(p_Device, layout, nullptr); }
        };
        if (p_Layout != VK_NULL_HANDLE) { vkDestroyPipelineLayout(p_Device, p_Layout, nullptr); }
        if (p_Pipeline != VK_NULL_HANDLE) { vkDestroyPipeline(p_Device, p_Pipeline, nullptr); }
    }

    VkPipeline Pipeline::GetPipeline() const { return p_Pipeline; }

    VkPipelineLayout Pipeline::GetLayout() const { return p_Layout; }

    const std::map<uint32_t, VkDescriptorSetLayout>& Pipeline::GetDescriptorLayouts() const
    {
        return p_DescriptorLayouts;
    }

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

        // set location -> descriptor set purposetype
        std::map<uint32_t, BufferResourceType> types;

        for (auto& resource: info->resources.bufferResources)
        {
            switch (resource.type)
            {
                case BufferResourceType::Buffer:
                case BufferResourceType::UniformBuffer:
                case BufferResourceType::StorageBuffer:
                    types[static_cast<uint32_t>(resource.layout.set)] = BufferResourceType::Buffer;
                    break;
                default:
                    break;
            }
        }
        for (auto& resource: info->resources.textureResources)
        {
            types[static_cast<uint32_t>(resource.layout.set)] = BufferResourceType::Texture;
        }

        for (auto& [set, type]: types) { builder.AddDescriptorSet(set, type); }

        auto pipelineData = builder.CreatePipeline();

        p_Pipeline = pipelineData.pipeline;
        p_Layout = pipelineData.layout;
        for (auto& [location, layout]: pipelineData.descriptorSetLayouts) { p_DescriptorLayouts[location] = layout; }
    }

    ComputePipeline::ComputePipeline(RendererDataType* rendererData, const ShaderInfo* info,
                                     const std::map<size_t, std::vector<uint32_t>>& shaderSources)
        : Pipeline(rendererData->device)
    {

        PipelineBuilder builder(rendererData, info);
        builder.SetPipelineType(PipelineType::Compute);
        builder.AddStage(ShaderStage::Compute, shaderSources.at(VK_SHADER_STAGE_COMPUTE_BIT));

        auto pipelineData = builder.CreatePipeline();

        p_Pipeline = pipelineData.pipeline;
        p_Layout = pipelineData.layout;
        for (auto& [location, layout]: pipelineData.descriptorSetLayouts) { p_DescriptorLayouts[location] = layout; }
    }
}// namespace LunaraEngine
