#pragma once

#include "Pipeline.hpp"

namespace LunaraEngine
{
    class SwapChain;

    class GraphicsPipeline: public Pipeline
    {
    public:
        GraphicsPipeline(VkDevice device, SwapChain* swapchain, const std::vector<uint32_t>& vertexSpriv,
                         const std::vector<uint32_t>& fragmentSpriv, const std::vector<VkDescriptorSetLayout>& layouts);
        ~GraphicsPipeline();
    };
}// namespace LunaraEngine
