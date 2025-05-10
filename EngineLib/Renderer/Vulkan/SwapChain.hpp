#pragma once
#include "vulkan/vulkan.h"
#include "VulkanDataTypes.hpp"
#include <vector>
#include <limits>

namespace LunaraEngine
{


    class SwapChain
    {
    public:
        SwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        ~SwapChain();

    public:
        // clang-format off
        void Create(VkExtent2D size);
        auto GetImageFormat() const { return m_SurfaceFormat.format; }
        auto GetSurfaceFormat() const { return m_SurfaceFormat; }
        auto GetExtent() const { return m_extent; }
        auto GetSwapChain() const { return m_swapChain; }
        auto GetImages() const { return m_Images; }
        auto GetImage(uint32_t index) const { return m_Images[index]; }
        auto GetImageViews() const { return m_ImageViews; }
        auto GetImageView(uint32_t index) const { return m_ImageViews[index]; }
        auto GetRenderPass() const { return m_renderPass; }

        // clang-format on

    private:
        void CreateImageViews();
        void CreateRenderPass();

    private:
        VkDevice m_device{};
        VkSwapchainKHR m_swapChain{};
        VkPhysicalDevice m_physicalDevice{};
        VkSurfaceKHR m_surface{};
        VkExtent2D m_extent{};
        VkRenderPass m_renderPass{};
        VkSurfaceFormatKHR m_SurfaceFormat{};
        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
    };
}// namespace LunaraEngine