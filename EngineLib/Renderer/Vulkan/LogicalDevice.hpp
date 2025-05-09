#include <vulkan/vulkan.h>
#include <set>
#include <vector>
#include "VulkanDataTypes.hpp"

namespace LunaraEngine
{
    class LogicalDevice
    {
    public:
        LogicalDevice(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue gfxQueue,
                      VkQueue presentQueue, VkQueue computeQueue);
    };
}// namespace LunaraEngine