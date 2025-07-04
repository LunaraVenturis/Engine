#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>

namespace LunaraEngine
{
    struct QueueFamilyIndices;
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

    bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);

}// namespace LunaraEngine