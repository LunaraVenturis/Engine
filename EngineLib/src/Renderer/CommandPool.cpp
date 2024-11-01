/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * CommandPool class implementation
 */


#include "CommandPool.hpp"

#include <vulkan/vulkan.hpp>

#include <Core/Log.hpp>

namespace Engine
{
    auto CommandPool::Create(VkDevice device, uint32_t graphicsQueueFamilyIndex, CommandPool* pCommandPool)
            -> std::expected<CommandPoolState, ErrorStatus>
    {

        pCommandPool->m_VkDevice = device;
        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        createInfo.flags = 0;

        auto result = vkCreateCommandPool(device, &createInfo, nullptr, &pCommandPool->m_VkCommandPool);

        if (VK_SUCCESS != result)
        {
            LOG_ERROR("Can Not Create Command Pool!\n");
            return std::unexpected(ErrorStatus::Fail);
        }
        LOG_INFO("Created Command Pool\n");

        pCommandPool->CreateCommandBuffer();
        return CommandPoolState::Created;
    }

    auto CommandPool::Destroy(CommandPool* pCommandPool) -> std::expected<CommandPoolState, ErrorStatus>
    {
        if (pCommandPool)
        {
            for (auto& [bufferState, buffer]: pCommandPool->m_VkCommandBuffers)
            {
                vkFreeCommandBuffers(pCommandPool->m_VkDevice, pCommandPool->m_VkCommandPool, 1, &buffer);
                bufferState = CommandBufferState::Invalid;
            }
            pCommandPool->Destroy();

            LOG_INFO("Destroyed Command Pool\n");
            return CommandPoolState::Destroyed;
        }
        return std::unexpected(ErrorStatus::Fail);
    }

    void CommandPool::Destroy() { vkDestroyCommandPool(m_VkDevice, m_VkCommandPool, nullptr); }

    auto CommandPool::CreateCommandBuffer() -> std::expected<CommandBufferState, ErrorStatus>
    {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = m_VkCommandPool;
        allocateInfo.commandBufferCount = 1;

        std::vector<VkCommandBuffer> buffers(1);
        auto result = vkAllocateCommandBuffers(m_VkDevice, &allocateInfo, buffers.data());
        if (VK_SUCCESS != result) { return std::unexpected(ErrorStatus::Fail); }

        auto buffer = buffers.front();
        m_VkCommandBuffers.emplace_back(ResultValue{CommandBufferState::Initial, buffer});

        return CommandBufferState::Created;
    }

    std::expected<VkCommandBuffer, ErrorStatus> CommandPool::GetCommandBuffer(uint32_t id)
    {
        if (m_VkCommandBuffers.size() <= id) { return std::unexpected(ErrorStatus::Invalid); }

        return m_VkCommandBuffers[id].value;
    }

    auto CommandPool::BeginCommandBuffer(uint32_t id) -> std::expected<CommandBufferState, ErrorStatus>
    {
        if (m_VkCommandBuffers.size() <= id) { return std::unexpected(ErrorStatus::Invalid); }

        if (CommandBufferState::Initial != m_VkCommandBuffers[id].status)
        {
            return std::unexpected(ErrorStatus::Invalid);
        }

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        auto bufferStatus = vkBeginCommandBuffer(m_VkCommandBuffers[id].value, &beginInfo);

        if (VK_SUCCESS != bufferStatus) { return std::unexpected(ErrorStatus::Invalid); }


        m_VkCommandBuffers[id].status = CommandBufferState::Recording;
        return CommandBufferState::Recording;
    }

    auto CommandPool::EndCommandBuffer(uint32_t id) -> std::expected<CommandBufferState, ErrorStatus>
    {
        if (m_VkCommandBuffers.size() <= id) { return std::unexpected(ErrorStatus::Invalid); }
        if (CommandBufferState::Recording != m_VkCommandBuffers[id].status)
        {
            return std::unexpected(ErrorStatus::Invalid);
        }

        auto bufferStatus = vkEndCommandBuffer(m_VkCommandBuffers[id].value);
        if (VK_SUCCESS != bufferStatus) { return std::unexpected(ErrorStatus::Invalid); }

        m_VkCommandBuffers[id].status = CommandBufferState::Executable;
        return CommandBufferState::Executable;
    }

}// namespace Engine