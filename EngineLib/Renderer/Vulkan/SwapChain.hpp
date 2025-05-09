#include "vulkan/vulkan.h"
#include "VulkanDataTypes.hpp"

namespace LunaraEngine
{
    class SwapChain
    {
    public:
        SwapChain(VkDevice device, VkSwapchainKHR swapChain, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t width, uint32_t height, std::vector<VkImage>& swapChainImages, VkFormat* swapChainImageFormat,  VkExtent2D* swapChainExtent);
    };
}// namespace LunaraEngine