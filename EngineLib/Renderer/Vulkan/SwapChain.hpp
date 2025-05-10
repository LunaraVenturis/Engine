#include "vulkan/vulkan.h"
#include <vector>
#include <limits>
namespace LunaraEngine
{
      struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
        inline SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicaldevice, VkSurfaceKHR surface);
    };

    inline SwapChainSupportDetails SwapChainSupportDetails::QuerySwapChainSupport(VkPhysicalDevice physicaldevice,
                                                                                  VkSurfaceKHR surface)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicaldevice, surface, &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, surface, &formatCount, details.formats.data());
        }
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, surface, &presentModeCount,
                                                      details.presentModes.data());
        }

        return details;
    }

    class SwapChain
    {
    public:
        SwapChain(VkDevice device, VkSwapchainKHR swapChain, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                  uint32_t width, uint32_t height, std::vector<VkImage>& swapChainImages,
                  VkFormat& swapChainImageFormat, VkExtent2D& swapChainExtent);
        ~SwapChain();

    private:
        VkDevice m_device;
        VkSwapchainKHR m_swapChain;
        VkPhysicalDevice m_physicalDevice;
        VkSurfaceKHR m_surface;
        uint32_t m_swapChainWidth;
        uint32_t m_swapChainHeight;
    };
}// namespace LunaraEngine