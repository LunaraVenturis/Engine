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

    private:
        void CreateDescriptorSets(RendererDataType* rendererData, const ShaderInfo& info);
        VkPipelineVertexInputStateCreateInfo
        CreateVertexInputInfo(const ShaderInfo& info, VkVertexInputBindingDescription& bindingDescription,
                              std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
        static VkDescriptorType GetDescriptorType(ShaderResourceType type);

    private:
        std::vector<VkDescriptorSetLayout> m_DescriptorSets;
    };
}// namespace LunaraEngine
