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
    };
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
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
}// namespace LunaraEngine