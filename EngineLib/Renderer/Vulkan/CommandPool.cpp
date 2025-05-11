#include "CommandPool.hpp"

namespace LunaraEngine
{
    CommandPool::CommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
        : m_device(device), m_physicalDevice(physicalDevice), m_surface(surface)
    {
        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_physicalDevice, m_surface);
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }
    CommandPool::~CommandPool()
    {
          vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }
  
}// namespace LunaraEngine