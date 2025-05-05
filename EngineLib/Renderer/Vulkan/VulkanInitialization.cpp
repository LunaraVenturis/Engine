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

    static std::vector<char> ReadFile(const std::string& name)
    {
        std::ifstream file(name, std::ios::ate | std::ios::binary);
        if (!file.is_open()) { throw std::runtime_error("failed to open file!"); }

        std::streamsize fileSize = file.tellg();
        if (fileSize < 0) { throw std::runtime_error("failed to determine file size!"); }
        std::vector<char> buffer(static_cast<size_t>(fileSize));
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    void VulkanInitializer::Initialize(RendererDataType* rendererData)
    {
        VulkanInitializer initializer(rendererData);
        initializer.CreateInstance();
        initializer.CreateSurface();
        initializer.PickPhysicalDevice();
        initializer.CreateLogicalDevice();
        initializer.CreateSwapChain();
        initializer.CreateImageViews();
        initializer.CreateGraphicsPipeline();
    }

    void VulkanInitializer::Goodbye(RendererDataType* rendererData)
    {
        vkDestroyPipeline(rendererData->device, rendererData->graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(rendererData->device, rendererData->pipelineLayout, nullptr);
        vkDestroyPipelineLayout(rendererData->device, rendererData->pipelineLayout, nullptr);
        vkDestroyRenderPass(rendererData->device, rendererData->renderPass, nullptr);
        vkDestroyPipelineLayout(rendererData->device, rendererData->pipelineLayout, nullptr);
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
        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        return indices.isComplete() && extensionsSupported && swapChainAdequate;
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
        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_RendererData->vkSurface, &formatCount,
                                                 details.formats.data());
        }
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_RendererData->vkSurface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_RendererData->vkSurface, &presentModeCount,
                                                      details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR
    VulkanInitializer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat: availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR
    VulkanInitializer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode: availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) { return availablePresentMode; }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanInitializer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D actualExtent = {static_cast<uint32_t>(m_RendererData->width),
                                       static_cast<uint32_t>(m_RendererData->height)};

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                            capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void VulkanInitializer::CreateSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_RendererData->physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_RendererData->vkSurface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = FindQueueFamilies(m_RendererData->physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;    // Optional
            createInfo.pQueueFamilyIndices = nullptr;// Optional
        }
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_RendererData->device, &createInfo, nullptr, &(m_RendererData->swapChain)) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(m_RendererData->device, m_RendererData->swapChain, &imageCount, nullptr);
        m_RendererData->swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_RendererData->device, m_RendererData->swapChain, &imageCount,
                                m_RendererData->swapChainImages.data());

        m_RendererData->swapChainImageFormat = surfaceFormat.format;
        m_RendererData->swapChainExtent = extent;
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

    VkShaderModule VulkanInitializer::CreateShaderModule(const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_RendererData->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }
        return shaderModule;
    }

    void VulkanInitializer::CreateGraphicsPipeline()
    {
        auto vertShaderCode = ReadFile("../../Shaders/output/vert.spv");
        auto fragShaderCode = ReadFile("../../Shaders/output/frag.spv");

        VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";


        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};


        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;// Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;// Optional

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) m_RendererData->swapChainExtent.width;
        viewport.height = (float) m_RendererData->swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_RendererData->swapChainExtent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;// Optional
        rasterizer.depthBiasClamp = 0.0f;         // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f;   // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;         // Optional
        multisampling.pSampleMask = nullptr;           // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE;// Optional
        multisampling.alphaToOneEnable = VK_FALSE;     // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;// Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;            // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;// Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;            // Optional

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;// Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;// Optional
        colorBlending.blendConstants[1] = 0.0f;// Optional
        colorBlending.blendConstants[2] = 0.0f;// Optional
        colorBlending.blendConstants[3] = 0.0f;// Optional


        std::array<VkDynamicState, 2> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;           // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr;        // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0;   // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr;// Optional

        if (vkCreatePipelineLayout(m_RendererData->device, &pipelineLayoutInfo, nullptr,
                                   &(m_RendererData->pipelineLayout)) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr;// Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_RendererData->pipelineLayout;
        pipelineInfo.renderPass = m_RendererData->renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;// Optional
        pipelineInfo.basePipelineIndex = -1;             // Optional

        if (vkCreateGraphicsPipelines(m_RendererData->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                      &(m_RendererData->graphicsPipeline)) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(m_RendererData->device, fragShaderModule, nullptr);
        vkDestroyShaderModule(m_RendererData->device, vertShaderModule, nullptr);
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
