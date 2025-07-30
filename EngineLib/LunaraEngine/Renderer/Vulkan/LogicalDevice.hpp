#include <vulkan/vulkan.h>
#include <set>
#include <vector>

namespace LunaraEngine
{
  
    class LogicalDevice
    {
    public:
        LogicalDevice(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue gfxQueue,
                      VkQueue presentQueue, VkQueue computeQueue);
    };
}// namespace LunaraEngine