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


    void VulkanInitializer::Initialize(RendererDataType* rendererData)
    {
        VulkanInitializer initializer(rendererData);
        initializer.CreateInstance();
        initializer.CreateSurface();
        initializer.PickPhysicalDevice();
        initializer.CreateLogicalDevice();
        initializer.CreateImageViews();
        initializer.CreateRenderPass();
    }

    void VulkanInitializer::Goodbye(RendererDataType* rendererData)
    {
        vkDestroyPipeline(rendererData->device, rendererData->graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(rendererData->device, rendererData->pipelineLayout, nullptr);
        vkDestroyRenderPass(rendererData->device, rendererData->renderPass, nullptr);
        for (const auto& imageView: rendererData->swapChainImageViews)
        {
            vkDestroyImageView(rendererData->device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(rendererData->device, rendererData->swapChain, nullptr);
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
            if (IsDeviceSuitable(device, m_RendererData->vkSurface))
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
    void VulkanInitializer::CreateImageViews()
    {
        m_RendererData->swapChainImageViews.resize(m_RendererData->swapChainImages.size());
        for (size_t i = 0; i < m_RendererData->swapChainImages.size(); ++i)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_RendererData->swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_RendererData->swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            if (vkCreateImageView(m_RendererData->device, &createInfo, nullptr,
                                  &(m_RendererData->swapChainImageViews[i])) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void VulkanInitializer::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_RendererData->swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(m_RendererData->device, &renderPassInfo, nullptr, &(m_RendererData->renderPass)) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }
}// namespace LunaraEngine
