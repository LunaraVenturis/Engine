#pragma once

#include "Pipeline.hpp"
#include <Renderer/CommonTypes.hpp>

namespace LunaraEngine
{
    class SwapChain;
    struct RendererDataType;

    class GraphicsPipeline: public Pipeline
    {
    public:
        GraphicsPipeline(RendererDataType* rendererData, const ShaderInfo* info,
                         const std::map<size_t, std::vector<uint32_t>>& shaderSources);
        ~GraphicsPipeline();

        VkDescriptorSetLayout GetDescriptorLayout() const { return m_DescriptorLayout; }

    public:
        static VkDescriptorType GetDescriptorType(ShaderResourceType type);

    private:
        void CreateDescriptorLayout(RendererDataType* rendererData, const ShaderInfo& info);
        VkPipelineVertexInputStateCreateInfo
        CreateVertexInputInfo(const ShaderInfo& info, VkVertexInputBindingDescription& bindingDescription,
                              std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
        std::vector<VkPushConstantRange> CreatePushConstantRanges(const ShaderInfo& info) const;

    private:
        VkDescriptorSetLayout m_DescriptorLayout;
    };
}// namespace LunaraEngine
