#include "VulkanInitialization.hpp"
#include "Renderer/Window.hpp"
#include "Common.hpp"
#include <Core/Log.h>
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
        QueueFamilyIndices indices = FindQueueFamilies(m_RendererData->physicalDevice, m_RendererData->vkSurface);

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

        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature = {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
                .pNext = NULL,
                .dynamicRendering = VK_TRUE,
        };

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = &dynamicRenderingFeature;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(g_SwapChainExtensions.size());
        createInfo.ppEnabledExtensionNames = g_SwapChainExtensions.data();

        if (vkCreateDevice(m_RendererData->physicalDevice, &createInfo, nullptr, &m_RendererData->device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        VkQueue queues[3];
        vkGetDeviceQueue(m_RendererData->device, indices.graphicsFamily.value(), 0, &queues[0]);
        vkGetDeviceQueue(m_RendererData->device, indices.presentFamily.value(), 0, &queues[1]);
        vkGetDeviceQueue(m_RendererData->device, indices.computeFamily.value(), 0, &queues[2]);

        m_RendererData->gfxQueue = Queue(queues[0], indices.graphicsFamily.value());
        m_RendererData->presentQueue = Queue(queues[1], indices.presentFamily.value());
        m_RendererData->computeQueue = Queue(queues[2], indices.computeFamily.value());
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
            bool status = IsDeviceSuitable(device, m_RendererData->vkSurface);
            if (status)
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


}// namespace LunaraEngine
