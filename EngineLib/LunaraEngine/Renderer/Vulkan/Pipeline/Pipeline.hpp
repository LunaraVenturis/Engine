#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <LunaraEngine/Renderer/CommonTypes.hpp>
#include <LunaraEngine/Renderer/Vulkan/VulkanDataTypes.hpp>

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

        const std::map<uint32_t, VkDescriptorSetLayout>& GetDescriptorLayouts() const;

    protected:
        VkPipeline p_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout p_Layout = VK_NULL_HANDLE;
        VkDevice p_Device = VK_NULL_HANDLE;
        std::map<uint32_t, VkDescriptorSetLayout> p_DescriptorLayouts;
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