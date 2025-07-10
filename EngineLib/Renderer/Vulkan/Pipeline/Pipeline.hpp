#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <Renderer/CommonTypes.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>

namespace LunaraEngine
{

    class Pipeline
    {
    public:
        Pipeline(VkDevice device);
        ~Pipeline();

    public:
        VkPipeline GetPipeline() const;
        VkPipelineLayout GetLayout() const;

        VkDescriptorSetLayout GetDescriptorLayout() const;

    protected:
        VkPipeline p_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout p_Layout = VK_NULL_HANDLE;
        VkDevice p_Device = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayout> p_DescriptorLayouts;
    };

    class GraphicsPipeline: public Pipeline
    {
    public:
        GraphicsPipeline(RendererDataType* rendererData, const ShaderInfo* info,
                         const std::map<size_t, std::vector<uint32_t>>& shaderSources);
        ~GraphicsPipeline() = default;
    };

    class ComputePipeline: public Pipeline
    {
    public:
        ComputePipeline(RendererDataType* rendererData, const ShaderInfo* info,
                        const std::map<size_t, std::vector<uint32_t>>& shaderSources);
        ~ComputePipeline() = default;
    };
}// namespace LunaraEngine