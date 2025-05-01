#include "VulkanInitialization.hpp"
#include "Renderer/Window.hpp"
#include "Core/Log.h"
#include <stdexcept>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#if defined(_LUNARA_ENGINE_X11)
#include <X11/Xlib.h>
#include <vulkan/vulkan_xlib.h>
const char* LUNARA_INSTANCE_EXTENSIONS[] = {VK_KHR_XLIB_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, nullptr};
#elif defined(_LUNARA_ENGINE_WAYLAND)
#include <wayland-client.h>
#include <vulkan/vulkan_wayland.h>
const char* LUNARA_INSTANCE_EXTENSIONS[] = {VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME,
                                            nullptr};
#elif defined(_LUNARA_ENGINE_WINDOWS)
#include <vulkan/vulkan_win32.h>
#include <Windows.h>

const char* LUNARA_INSTANCE_EXTENSIONS[] = {VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                            VK_KHR_SURFACE_EXTENSION_NAME, nullptr};
#else
#error "Unsupported platform"
#endif

#ifdef _DEBUG
const char* LUNARA_INSTANCE_LAYERS[] = {"VK_LAYER_KHRONOS_validation", nullptr};
#else
const char* LUNARA_INSTANCE_LAYERS[] = {nullptr};
#endif

namespace LunaraEngine
{

    const std::array<const char*, 1> g_SwapChainExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    void VulkanInitializer::Initialize(RendererDataType* rendererData)
    {
        VulkanInitializer initializer(rendererData);
        initializer.CreateInstance();
        initializer.CreateSurface();
        initializer.PickPhysicalDevice();
        initializer.CreateLogicalDevice();
    }

    void VulkanInitializer::Goodbye(RendererDataType* rendererData)
    {
        vkDestroySurfaceKHR(rendererData->instance, rendererData->vkSurface, nullptr);
        vkDestroyDevice(rendererData->device, nullptr);
        rendererData->debugMessanger.Destroy(rendererData->instance);
        vkDestroyInstance(rendererData->instance, nullptr);
    }

    void VulkanInitializer::CreateInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Name";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "LunaraEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_MAKE_VERSION(1, 3, 0);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        std::array<const char*, 10> extensions;
        uint32_t extCount = 0;
        while (const char* extension = LUNARA_INSTANCE_EXTENSIONS[extCount]) { extensions[extCount++] = extension; }

        std::array<const char*, 10> layers;
        uint32_t layerCount = 0;
        while (const char* layer = LUNARA_INSTANCE_LAYERS[layerCount]) { layers[layerCount++] = layer; }

        createInfo.enabledExtensionCount = extCount;
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = layerCount;
        createInfo.ppEnabledLayerNames = layers.data();

        VkInstance* instance = &(m_RendererData->instance);

        auto result = vkCreateInstance(&createInfo, nullptr, instance);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create instance! Error code: " + std::to_string(result));
        }

        m_RendererData->debugMessanger.Create(m_RendererData->instance);
    }

    void VulkanInitializer::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(m_RendererData->physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(),
                                                  indices.computeFamily.value()};
        float queuePriority = 1.0f;

        for (uint32_t queueFamily: uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(g_SwapChainExtensions.size());
        createInfo.ppEnabledExtensionNames = g_SwapChainExtensions.data();

        if (vkCreateDevice(m_RendererData->physicalDevice, &createInfo, nullptr, &m_RendererData->device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }
        vkGetDeviceQueue(m_RendererData->device, indices.graphicsFamily.value(), 0, &m_RendererData->gfxQueue);
        vkGetDeviceQueue(m_RendererData->device, indices.presentFamily.value(), 0, &m_RendererData->presentQueue);
        vkGetDeviceQueue(m_RendererData->device, indices.computeFamily.value(), 0, &m_RendererData->computeQueue);
    }

    void VulkanInitializer::CreateSurface()
    {
        auto windowPtr = static_cast<SDL_Window*>(m_RendererData->window->data);
        auto instance = m_RendererData->instance;
        auto* surface = &m_RendererData->vkSurface;
        if (!(SDL_Vulkan_CreateSurface(windowPtr, instance, nullptr, surface)))
        {
            throw std::runtime_error("failed to create surface!");
        }
    }

    void VulkanInitializer::PickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_RendererData->instance, &deviceCount, nullptr);

        if (deviceCount == 0) { throw std::runtime_error("failed to find GPUs with Vulkan support!"); }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_RendererData->instance, &deviceCount, devices.data());

        for (const auto& device: devices)
        {
            if (IsDeviceSuitable(device))
            {
                m_RendererData->physicalDevice = device;
                break;
            }
        }

        if (m_RendererData->physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    bool VulkanInitializer::IsDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);
        bool extensionsSupported = CheckDeviceExtensionSupport(device);
        return indices.isComplete() && extensionsSupported;
    }

    QueueFamilyIndices VulkanInitializer::FindQueueFamilies(VkPhysicalDevice device)
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
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_RendererData->vkSurface, &presentSupport);

            if (presentSupport) { indices.presentFamily = i; }
            if (indices.isComplete()) { break; }

            i++;
        }

        return indices;
    }

    bool VulkanInitializer::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(g_SwapChainExtensions.begin(), g_SwapChainExtensions.end());

        for (const auto& extension: availableExtensions) { requiredExtensions.erase(extension.extensionName); }

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails VulkanInitializer::QuerySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_RendererData->vkSurface, &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_RendererData->vkSurface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_RendererData->vkSurface, &formatCount, details.formats.data());
        }
        return {};
    }
}// namespace LunaraEngine