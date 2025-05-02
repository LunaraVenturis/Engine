#pragma once
#include <Core/STDTypes.h>
#include <Renderer/RendererAPI.hpp>
#include <Renderer/RendererCommands.hpp>
#include <Renderer/Window.hpp>
#include <Renderer/Vulkan/VulkanDebugMessanger.hpp>
#include <vulkan/vulkan.h>
#include <SDL3/SDL_render.h>

#include <vector>
#include <optional>
#include <cstdint>
#include <array>
#include <set>
#include <limits>   // Necessary for std::numeric_limits
#include <algorithm>// Necessary for std::clamp

namespace LunaraEngine
{
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

}// namespace LunaraEngine