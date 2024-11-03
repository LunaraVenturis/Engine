#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * Command Pool class definition 
 */

#include <expected>

#include <vulkan/vulkan.h>

#include <Core/Result.hpp>
#include <Core/Error.hpp>

namespace Engine
{
    enum class CommandPoolState
    {
        Destroyed,
        Created
    };

    enum class CommandBufferState
    {
        Created,
        Destroyed,
        Initial,
        Recording,
        Executable,
        Pending,
        Invalid
    };
}// namespace Engine

namespace Engine
{

    class CommandPool
    {
    public:
        CommandPool() = default;
        ~CommandPool() = default;

    public:
        std::expected<CommandBufferState, ErrorStatus> CreateCommandBuffer();
        std::expected<VkCommandBuffer, ErrorStatus> GetCommandBuffer(uint32_t id = 0);

        std::expected<CommandBufferState, ErrorStatus> BeginCommandBuffer(uint32_t id = 0);
        std::expected<CommandBufferState, ErrorStatus> EndCommandBuffer(uint32_t id = 0);

    public:
        static std::expected<CommandPoolState, ErrorStatus> Create(VkDevice device, uint32_t graphicsQueueFamilyIndex,
                                                                   CommandPool* pCommandPool);

        static std::expected<CommandPoolState, ErrorStatus> Destroy(CommandPool* pCommandPool);

    public:
        operator VkCommandPool() const { return m_VkCommandPool; }

    private:
        void Destroy();

    private:
        VkCommandPool m_VkCommandPool;
        std::vector<ResultValue<CommandBufferState, VkCommandBuffer>> m_VkCommandBuffers;
        VkDevice m_VkDevice;
    };
}// namespace Engine