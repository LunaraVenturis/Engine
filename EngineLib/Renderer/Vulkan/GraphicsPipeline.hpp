#pragma once

#include "Pipeline.hpp"

namespace LunaraEngine
{
    class GraphicsPipeline: public Pipeline
    {
    public:
        GraphicsPipeline(VkDevice device, const std::vector<uint32_t>& vertexSpriv,
                         const std::vector<uint32_t>& fragmentSpriv, VkExtent2D viewportSize,
                         const std::vector<VkDescriptorSetLayout>& layouts, VkRenderPass renderPass);
    };
}// namespace LunaraEngine
