#pragma once
#include <vulkan/vulkan.h>
#include "Common.hpp"

namespace LunaraEngine
{

    class CommandPool
    {
    public:
        CommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        ~CommandPool();

    public:
        [[nodiscard]] auto GetCommandPool() const { return m_commandPool; }

    private:
        VkDevice m_device{};
        VkCommandPool m_commandPool{};
        VkPhysicalDevice m_physicalDevice{};
        VkSurfaceKHR m_surface{};
    };

}// namespace LunaraEngine