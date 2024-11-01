#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * VulkanInstance class definition
 */


#include "CommandPool.hpp"
#include "Image.hpp"
#include "VulkanSpec.hpp"
#include <Core/Result.hpp>

namespace Engine
{
    enum class VulkanContextStatus
    {
        Fail,
        Created,
        Destroyed
    };
    enum class VulkanDeviceStatus
    {
        Fail,
        Created,
        Destroyed
    };
    enum class VulkanPhysicalDeviceStatus
    {
        Unknown,
        Selected,
        Not_Found
    };
    enum class VulkanInstanceStatus
    {
        Success,
        Fail,
        Created,
        Destroyed
    };
    enum class VulkanSwapchainStatus
    {
        Success,
        Fail,
        Created,
        Destroyed
    };
    enum class VulkanQueueFamilyStatus
    {
        Found,
        Not_Found,
    };
}// namespace Engine

namespace Engine
{
    class Window;

    class VulkanContext
    {
    public:
        VulkanContext() = default;
        VulkanContext(VulkanSpec spec, Window* windowPtr);
        ~VulkanContext() = default;

    private:
        ResultValueType<VulkanInstanceStatus> CreateInstance();
        ResultValueType<VulkanPhysicalDeviceStatus> SelectPhysicalDevice();
        ResultValueType<VulkanDeviceStatus> CreateDevice();
        ResultValueType<VulkanQueueFamilyStatus> SelectQueueFamily();
        ResultValueType<VulkanSwapchainStatus> CreateSwapchain();
        ResultValueType<VulkanSwapchainStatus> GetCapabilities();

    private:
        Window* m_WindowPtr{};
        VulkanSpec m_Spec{};
        VkInstance m_Instance{};
        VkPhysicalDevice m_PhysicalDevice{};
        VkDevice m_Device{};
        VkSwapchainKHR m_Swapchain{};
        VkSurfaceKHR m_Surface{};
        CommandPool* m_CommandPool{};
        uint32_t m_GraphicsQueueIndex{};
        uint32_t m_PresentQueueIndex{};
        std::vector<VkSurfaceFormatKHR> m_SupportedFormats;
        VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
        VkFormat m_Format;
        VkExtent2D m_SwapchainExtent;
        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
        Image<> m_DepthImage;

    public:
        static ResultValueType<VulkanContextStatus> Create(VulkanSpec spec, Window* windowPtr);
        static ResultValueType<VulkanContextStatus> Destroy();
        static VulkanContext* Get();

    private:
        static VulkanContext* s_VulkanContext;
    };
}// namespace Engine