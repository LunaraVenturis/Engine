#pragma once
#include <vulkan/vulkan.h>
#include "CommandBuffer.hpp"
#include <vector>
#include <memory>

namespace LunaraEngine
{

    class CommandPool
    {
    public:
        CommandPool(VkDevice device, uint32_t queueFamilyIndex, uint32_t preallocateBufferCount = 2);
        ~CommandPool();

    public:
        void AllocateCommandBuffers(uint32_t count = 1);
        void DestroyCommandBuffers();

        std::unique_ptr<CommandBuffer> CreateImmediateCommandBuffer();

    public:
        [[nodiscard]] auto GetPool() const { return m_commandPool; }

        [[nodiscard]] auto& GetBuffers() const { return m_commandBuffers; }

        [[nodiscard]] auto GetBuffer(uint32_t index) const { return m_commandBuffers[index]; }

    private:
        VkDevice m_device{};
        VkCommandPool m_commandPool{};
        uint32_t m_queueFamilyIndex{};
        std::vector<CommandBuffer> m_commandBuffers{};
    };

}// namespace LunaraEngine