#pragma once
#include <cstdint>
#include "VulkanDataTypes.hpp"

namespace LunaraEngine
{

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

    bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

    
}// namespace LunaraEngine