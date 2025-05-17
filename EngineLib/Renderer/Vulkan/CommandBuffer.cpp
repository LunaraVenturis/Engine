#include "CommandBuffer.hpp"
#include <stdexcept>
#include <Core/Log.h>

namespace LunaraEngine
{
    CommandBuffer::CommandBuffer(VkDevice device, VkCommandPool cmdPool) : m_device(device), m_cmdPool(cmdPool)
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

    CommandBuffer::CommandBuffer(CommandBuffer&& other)
        : m_device(other.m_device), m_cmdBuffer(other.m_cmdBuffer), m_cmdPool(other.m_cmdPool)
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

    CommandBuffer::~CommandBuffer() { Destroy(); }

    void CommandBuffer::Destroy() { vkFreeCommandBuffers(m_device, m_cmdPool, 1, &m_cmdBuffer); }

    void CommandBuffer::BeginRecording() const
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

    void CommandBuffer::EndRecording() const
    {

        if (vkEndCommandBuffer(m_cmdBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void CommandBuffer::Flush(VkQueue queue)
    {
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_cmdBuffer;
        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    }

    bool CommandBuffer::IsValid() const { return m_cmdBuffer != VK_NULL_HANDLE; }

    CommandBuffer::operator VkCommandBuffer() const { return m_cmdBuffer; }
}// namespace LunaraEngine
