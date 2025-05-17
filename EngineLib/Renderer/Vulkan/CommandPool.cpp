#include "CommandPool.hpp"
#include <stdexcept>
#include <memory>

namespace LunaraEngine
{
    CommandPool::CommandPool(VkDevice device, uint32_t queueFamilyIndex, uint32_t preallocateBufferCount)
        : m_device(device), m_queueFamilyIndex(queueFamilyIndex)
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = m_queueFamilyIndex;

        if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }

        AllocateCommandBuffers(preallocateBufferCount);
    }

    CommandPool::~CommandPool()
    {
        DestroyCommandBuffers();
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }

    void CommandPool::AllocateCommandBuffers(uint32_t count)
    {
        if (count == 0) { return; }
        if (m_commandBuffers.size() > 0) { DestroyCommandBuffers(); }
        for (size_t i = 0; i < count; i++) { m_commandBuffers.emplace_back(m_device, m_commandPool); }
    }

    void CommandPool::DestroyCommandBuffers()
    {
        for (auto& commandBuffer: m_commandBuffers) { commandBuffer.Destroy(); }
        m_commandBuffers.clear();
    }

    std::unique_ptr<CommandBuffer> CommandPool::CreateImmediateCommandBuffer()
    {
        return std::make_unique<CommandBuffer>(m_device, m_commandPool);
    }

}// namespace LunaraEngine