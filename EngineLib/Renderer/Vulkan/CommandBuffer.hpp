#pragma once
#include <vulkan/vulkan.h>
#include "CommandPool.hpp"
#include "SwapChain.hpp"
#include "GraphicsPipeline.hpp"

namespace LunaraEngine
{
    class CommandBuffer
    {
    public:
        CommandBuffer(VkDevice device, VkCommandBuffer cmdBuffer, const CommandPool& cmdPool);
        ~CommandBuffer() = default;

    public:
        void RecordCmdBuffer(const SwapChain& swapChain, uint32_t imgIdx);
        void BindPipeline(const GraphicsPipeline& gfxPipeline, const SwapChain& swapChain);

    private:
        VkDevice m_device{};
        VkCommandBuffer m_cmdBuffer{};
    };
}// namespace LunaraEngine