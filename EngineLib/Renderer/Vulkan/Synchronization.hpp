#pragma once
#include <vulkan/vulkan.h>

namespace LunaraEngine
{
    enum class FenceState
    {
        NotSignaled,
        Signaled
    };

    class VulkanFence
    {
    public:
        VulkanFence() = default;
        VulkanFence(VkDevice device, FenceState state = FenceState::NotSignaled);
        ~VulkanFence() = default;

    public:
        void Wait(size_t period_ns = UINT64_MAX) const;
        void Reset();
        void Destroy();

    public:
        VkFence GetFence() const { return m_Fence; }

        operator VkFence() const { return m_Fence; }
        
        FenceState GetFenceState() const { return m_State; }
    private:
        static VkFenceCreateFlagBits GetFenceState(FenceState state);

    private:
        VkDevice m_Device{};
        VkFence m_Fence{};
        FenceState m_State{};
    };
}// namespace LunaraEngine