#pragma once
#include <Core/STDTypes.h>
#include <Renderer/RendererAPI.hpp>
#include <Renderer/RendererCommands.hpp>
#include <Renderer/Window.hpp>
#include <Renderer/Vulkan/VulkanDebugMessanger.hpp>
#include "Pipeline.hpp"
#include "SwapChain.hpp"
#include "CommandPool.hpp"
#include "Queue.hpp"
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
        LunaraEngine::Window* window;
        SDL_Renderer* renderer;
        SDL_Surface* surface;
        VkInstance instance;
        VulkanDebugMessanger debugMessanger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;
        VkExtent2D surfaceExtent;
        Queue gfxQueue;
        VkSurfaceKHR vkSurface;
        Queue presentQueue;
        Queue computeQueue;
        Pipeline* pipeline;
        SwapChain* swapChain;
        CommandPool* commandPool;
        uint32_t currentFrame;
        uint32_t imageIndex;
        uint32_t maxFramesInFlight;
        VkClearValue clearValue;
        std::vector<VkSemaphore> imageAvailableSemaphore;
        std::vector<VkSemaphore> renderFinishedSemaphore;
        std::vector<VkFence> inFlightFence;
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
        static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicaldevice, VkSurfaceKHR surface);
    };

}// namespace LunaraEngine