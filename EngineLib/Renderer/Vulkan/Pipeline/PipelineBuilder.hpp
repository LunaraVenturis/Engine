#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "Pipeline.hpp"

namespace LunaraEngine
{
    class RendererDataType;
    class ShaderInfo;
    enum class RenderingBasePrimitive;

    class PipelineBuilder
    {
    public:
        PipelineBuilder() = default;
        ~PipelineBuilder() = default;

        PipelineBuilder(RendererDataType* rendererData, const ShaderInfo* info);

        void SetPipelineType(PipelineType type);
        void AddDescriptorSetLayout(VkDescriptorSetLayout layout);
        void AddStage(ShaderStage stage, const std::vector<uint32_t>& spirvCode);
        void AddVertexInputInfo();
        void SetInputAssembly(RenderingBasePrimitive primitive = RenderingBasePrimitive::TRIANGLES);
        void SetViewportState(std::vector<VkViewport> viewports = {}, std::vector<VkRect2D> scissors = {});
        void SetRasterization(PolygonMode mode);
        void SetSampling();
        void AddColorBlending(VkPipelineColorBlendStateCreateInfo colorBlending);
        void SetDynamicStates();
        VkPipeline CreatePipeline();

    private:
        VkShaderModule CreateShaderModule(const std::vector<uint32_t>& spirvCode) const;
        VkPrimitiveTopology GetPrimitiveTopology(RenderingBasePrimitive primitive) const;
        VkPolygonMode GetPolygonMode(PolygonMode mode) const;

    private:
        RendererDataType* m_RendererData{};
        const ShaderInfo* m_Info{};
        PipelineType m_PipelineType{};
        VkVertexInputBindingDescription m_BindingDescription{};
        VkPipelineVertexInputStateCreateInfo m_VertexInputInfo{};
        VkDescriptorSetLayout m_DescriptorLayout{};
        VkPipelineInputAssemblyStateCreateInfo m_InputAssembly{};
        VkPipelineViewportStateCreateInfo m_ViewportState{};
        VkPipelineRasterizationStateCreateInfo m_Rasterizer{};
        VkPipelineMultisampleStateCreateInfo m_Multisampling{};
        VkPipelineColorBlendStateCreateInfo m_ColorBlending{};

        std::vector<VkShaderModule> m_ShaderModules;
        std::vector<VkShaderStageFlagBits> m_ShaderStageFlags;
        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
        std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
        std::vector<VkPipelineColorBlendAttachmentState> m_BlendAttachments;
    };
}// namespace LunaraEngine