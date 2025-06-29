#include "Synchronization.hpp"
#include <stdexcept>

namespace LunaraEngine
{
    VulkanFence::VulkanFence(VkDevice device, FenceState state) : m_Device(device), m_State(state)
    {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = GetFenceState(state);
        auto result = vkCreateFence(m_Device, &fenceInfo, nullptr, &m_Fence);
        if (result != VK_SUCCESS) { throw std::runtime_error("failed to create fence!"); }
    }

    void VulkanFence::Wait(size_t period_ns) const
    {
        auto result = vkWaitForFences(m_Device, 1, &m_Fence, VK_TRUE, (uint64_t) period_ns);
        if (result != VK_SUCCESS) { throw std::runtime_error("failed to wait for fence!"); }
    }

    void VulkanFence::Reset()
    {
        auto result = vkResetFences(m_Device, 1, &m_Fence);
        if (result != VK_SUCCESS) { throw std::runtime_error("failed to reset fence!"); }
    }

    void VulkanFence::Destroy() { vkDestroyFence(m_Device, m_Fence, nullptr); }

    VkFenceCreateFlagBits VulkanFence::GetFenceState(FenceState state)
    {
        switch (state)
        {
            case FenceState::NotSignaled:
                return {};
            case FenceState::Signaled:
                return VK_FENCE_CREATE_SIGNALED_BIT;
            default:
                throw std::runtime_error("invalid fence state!");
                break;
        }
        return {};
    }
}// namespace LunaraEngine