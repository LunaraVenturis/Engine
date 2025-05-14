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
        CommandBuffer(VkDevice device, VkQueue queue, VkCommandBuffer cmdBuffer, const CommandPool& cmdPool);
        ~CommandBuffer() = default;

    public:
        void BeginRecording();
        void Draw();
        void EndRecording();
        void Submit();

    private:
        VkDevice m_device{};
        VkQueue m_queue{};
        VkCommandBuffer m_cmdBuffer{};
        VkCommandPool m_cmdPool{};
    };
}// namespace LunaraEngine