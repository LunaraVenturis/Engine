#pragma once
#include <Core/STDTypes.h>
#include <Renderer/RendererAPI.hpp>
#include <Renderer/RendererCommands.hpp>
#include <Renderer/Window.hpp>
#include <Renderer/Vulkan/VulkanDebugMessanger.hpp>
#include "Pipeline.hpp"
#include <vulkan/vulkan.h>
#include <SDL3/SDL_render.h>
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <array>
#include <set>
#include <limits>   // Necessary for std::numeric_limits
#include <algorithm>// Necessary for std::clamp
#include <fstream>

namespace LunaraEngine
{
    const std::array<const char*, 1> g_SwapChainExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    struct RendererDataType {
        std::vector<RendererCommand*> command_stack;
        LunaraEngine::Window* window;
        SDL_Renderer* renderer;
        SDL_Surface* surface;
        VkInstance instance;
        VulkanDebugMessanger debugMessanger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;
        VkQueue gfxQueue;
        VkSurfaceKHR vkSurface;
        VkQueue presentQueue;
        VkQueue computeQueue;
        VkSwapchainKHR swapChain;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;
        VkPipelineLayout pipelineLayout;
        VkRenderPass renderPass;
        VkPipeline graphicsPipeline;
        Pipeline* pipeline;
        float width;
        float height;
    };

    struct QueueFamilyIndices;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> computeFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const
        {
            return graphicsFamily.has_value() && computeFamily.has_value() && presentFamily.has_value();
        }
    };

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

    inline QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        uint32_t i = 0;

        for (const auto& queueFamily: queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) { indices.graphicsFamily = i; }
            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) { indices.computeFamily = i; }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) { indices.presentFamily = i; }
            if (indices.isComplete()) { break; }

            i++;
        }

        return indices;
    }

    inline bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(g_SwapChainExtensions.begin(), g_SwapChainExtensions.end());

        for (const auto& extension: availableExtensions) { requiredExtensions.erase(extension.extensionName); }

        return requiredExtensions.empty();
    }

    inline bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device, surface);
        bool extensionsSupported = CheckDeviceExtensionSupport(device);
        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport;
            swapChainSupport.QuerySwapChainSupport(device, surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

}// namespace LunaraEngine