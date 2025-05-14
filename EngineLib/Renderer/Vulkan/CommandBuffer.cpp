#include "CommandBuffer.hpp"

namespace LunaraEngine
{
    CommandBuffer::CommandBuffer(VkDevice device, VkQueue queue, VkCommandBuffer cmdBuffer, const CommandPool& cmdPool)
        : m_device(device), m_queue(queue), m_cmdBuffer(cmdBuffer), m_cmdPool(cmdPool.GetCommandPool())
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_cmdPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(m_device, &allocInfo, &m_cmdBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void CommandBuffer::BeginRecording()
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;                 // Optional
        beginInfo.pInheritanceInfo = nullptr;// Optional

        if (vkBeginCommandBuffer(m_cmdBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
    }

    void CommandBuffer::Draw() { vkCmdDraw(m_cmdBuffer, 3, 1, 0, 0); }

    void CommandBuffer::EndRecording()
    {

        if (vkEndCommandBuffer(m_cmdBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void CommandBuffer::Submit()
    {
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_cmdBuffer;
        vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE);
    }
}// namespace LunaraEngine
