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
                   void Create(VkExtent2D size);
     [[nodiscard]] auto GetImageFormat() const { return m_SurfaceFormat.format; }
     [[nodiscard]] auto GetSurfaceFormat() const { return m_SurfaceFormat; }
     [[nodiscard]] auto GetExtent() const { return m_extent; }
     [[nodiscard]] auto GetSwapChain() const { return m_swapChain; }
     [[nodiscard]] auto GetImages() const { return m_Images; }
     [[nodiscard]] auto GetImage(uint32_t index) const { return m_Images[index]; }
     [[nodiscard]] auto GetImageViews() const { return m_ImageViews; }
     [[nodiscard]] auto GetImageView(uint32_t index) const { return m_ImageViews[index]; }
     [[nodiscard]] auto GetRenderPass() const { return m_renderPass; }
     [[nodiscard]] auto GetFrameBuffer(uint32_t index) const {return m_swapChainFrameBuffer[index];}
    private:
        void CreateImageViews();
        void CreateRenderPass();
        void CreateFrameBuffers();
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
        std::vector<VkFramebuffer> m_swapChainFrameBuffer;
    };
}// namespace LunaraEngine