#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "Pipeline.hpp"
#include <LunaraEngine/Renderer/CommonTypes.hpp>

namespace LunaraEngine
{

    class PipelineBuilder
    {
    public:
        PipelineBuilder() = default;
        ~PipelineBuilder() = default;

        PipelineBuilder(RendererDataType* rendererData, const ShaderInfo* info);

        void SetPipelineType(PipelineType type);
        void AddStage(ShaderStage stage, const std::vector<uint32_t>& spirvCode);
        void AddVertexInputInfo();
        void SetInputAssembly(RenderingBasePrimitive primitive = RenderingBasePrimitive::TRIANGLES);
        void SetViewportState(std::vector<VkViewport> viewports = {}, std::vector<VkRect2D> scissors = {});
        void SetRasterization(PolygonMode mode);
        void SetSampling();
        void AddColorBlending();
        void AddDynamicState(VkDynamicState state);
        void AddDescriptorSet(uint32_t set, BufferResourceType type);
        PipelineData CreatePipeline();
        static VkDescriptorType GetDescriptorType(BufferResourceType type);

    private:
        void CreateDescriptorSetLayout(uint32_t location, BufferResourceType type);
        void GetBufferDescriptorLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
        void GetTextureDescriptorLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
        void CreatePushConstantRanges();
        void CreatePipelineLayout();
        VkShaderModule CreateShaderModule(const std::vector<uint32_t>& spirvCode) const;
        VkPrimitiveTopology GetPrimitiveTopology(RenderingBasePrimitive primitive) const;
        VkPolygonMode GetPolygonMode(PolygonMode mode) const;

    private:
        RendererDataType* m_RendererData{};
        const ShaderInfo* m_Info{};
        PipelineType m_PipelineType{};
        VkVertexInputBindingDescription m_BindingDescription{};
        VkPipelineVertexInputStateCreateInfo m_VertexInputInfo{};
        VkPipelineInputAssemblyStateCreateInfo m_InputAssembly{};
        VkPipelineViewportStateCreateInfo m_ViewportState{};
        VkPipelineRasterizationStateCreateInfo m_Rasterizer{};
        VkPipelineMultisampleStateCreateInfo m_Multisampling{};
        VkPipelineColorBlendStateCreateInfo m_ColorBlending{};
        VkPipelineDynamicStateCreateInfo m_DynamicState{};
        VkPipelineLayout m_Layout{};

        std::vector<VkPushConstantRange> m_ConstantRanges{};
        std::vector<VkShaderModule> m_ShaderModules;
        std::vector<VkShaderStageFlagBits> m_ShaderStageFlags;
        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
        std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
        std::vector<VkPipelineColorBlendAttachmentState> m_BlendAttachments;
        std::vector<VkDynamicState> m_DynamicStates;
        std::map<uint32_t, VkDescriptorSetLayout> m_DescriptorLayouts{};
        std::map<uint32_t, BufferResourceType> m_DescriptorSetTypes;
    };
}// namespace LunaraEngine