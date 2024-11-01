/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * VulkanContext class implementation
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "VulkanContext.hpp"
#include "DebugMessanger.hpp"
#include "Window.hpp"

#include <Core/EngineInfo.hpp>
#include <Core/Log.hpp>
#include <Renderer/Shader.hpp>

#include <shaderc/shaderc.hpp>

#include <algorithm>
#include <ranges>

Engine::VulkanContext* Engine::VulkanContext::s_VulkanContext = {};

namespace Engine
{

    ResultValueType<VulkanContextStatus> VulkanContext::Create(VulkanSpec spec, Window* windowPtr)
    {
        VulkanContext::s_VulkanContext = new VulkanContext(spec, windowPtr);
        auto vulkanContextPtr = VulkanContext::Get();

        //Create Instance
        if (VulkanInstanceStatus::Created != vulkanContextPtr->CreateInstance())
        {
            LOG_ERROR("Could not create Vulkan Instance!\n");
            return {VulkanContextStatus::Fail};
        }

        //Create Debug Callback
        DebugMessanger::Create(VulkanContext::Get()->m_Instance);

        //Create Window Surface
        auto surfaceStatus = windowPtr->CreateSurface(vulkanContextPtr->m_Instance);
        if (WindowStatus::Surface_Created != surfaceStatus)
        {
            LOG_ERROR("Could Not Create Window Surface!\n");
            return {VulkanContextStatus::Fail};
        }
        vulkanContextPtr->m_Surface = windowPtr->GetSurface().value;
        LOG_INFO("Created Window Surface!\n");

        //Select Physical Device
        if (VulkanPhysicalDeviceStatus::Selected != vulkanContextPtr->SelectPhysicalDevice())
        {
            return {VulkanContextStatus::Fail};
        }

        //Create Logical Device
        if (VulkanDeviceStatus::Created != vulkanContextPtr->CreateDevice()) { return {VulkanContextStatus::Fail}; }

        if (VulkanSwapchainStatus::Created != vulkanContextPtr->CreateSwapchain())
        {
            return {VulkanContextStatus::Fail};
        }

        //Create Command Pool
        VulkanContext::Get()->m_CommandPool = new CommandPool();
        auto result = CommandPool::Create(vulkanContextPtr->m_Device, vulkanContextPtr->m_GraphicsQueueIndex,
                                          VulkanContext::Get()->m_CommandPool);

        Shader shader;

        const std::string source_name = "shader_src";
        shaderc_shader_kind kind = shaderc_glsl_vertex_shader;
        const std::string source = "#version 460 core\n"
                                   "void main() { }\n";
        shader.CreateFromString(source, source);
        shader.Compile();

        // shaderc::Compiler compiler;
        // shaderc::CompileOptions options;

        // options.SetOptimizationLevel( shaderc_optimization_level_size );
        // options.AddMacroDefinition( "MY_DEFINE", "1" );
        // shaderc::AssemblyCompilationResult compResult =
        //         compiler.CompileGlslToSpvAssembly( source, kind, source_name.c_str(), options );

        // if ( compResult.GetCompilationStatus() != shaderc_compilation_status_success )
        // {
        //     std::cerr << compResult.GetErrorMessage();
        // }
        // std::string assembly( compResult.begin(), compResult.end() );
        // std::cout << "Optimized SPIR-V assembly:" << std::endl << assembly << std::endl;

        // shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv( source, kind, source_name.c_str(), options );

        // if ( module.GetCompilationStatus() != shaderc_compilation_status_success )
        // {
        //     std::cerr << module.GetErrorMessage();
        // }
        // std::string spirv( module.begin(), module.end() );
        // std::cout << "Compiled to an optimized binary module with " << spirv.size() << " words." << std::endl;


        LOG_INFO("Vulkan Context Created!\n");
        return {VulkanContextStatus::Created};
    }

    ResultValueType<VulkanContextStatus> VulkanContext::Destroy()
    {
        auto vulkanCtxPtr = VulkanContext::Get();

        CommandPool::Destroy(vulkanCtxPtr->m_CommandPool);
        delete vulkanCtxPtr->m_CommandPool;

        for (auto imageView: vulkanCtxPtr->m_ImageViews)
        {
            vkDestroyImageView(vulkanCtxPtr->m_Device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(vulkanCtxPtr->m_Device, vulkanCtxPtr->m_Swapchain, nullptr);
        LOG_INFO("Destroyed Swapchain!\n");

        vkDestroyDevice(vulkanCtxPtr->m_Device, nullptr);
        LOG_INFO("Destroyed Logical Device\n");

        auto windowSurfaceStatus = vulkanCtxPtr->m_WindowPtr->DestroySurface(vulkanCtxPtr->m_Instance);
        if (WindowStatus::Surface_Destroyed == windowSurfaceStatus) { LOG_INFO("Destroyed Window Surface\n"); }

        DebugMessanger::Destroy(vulkanCtxPtr->m_Instance);

        vkDestroyInstance(vulkanCtxPtr->m_Instance, nullptr);
        LOG_INFO("Destroyed Vulkan Instance\n");

        delete VulkanContext::s_VulkanContext;
        LOG_INFO("Vulkan Context Destroyed!\n");

        return {VulkanContextStatus::Destroyed};
    }

    VulkanContext* VulkanContext::Get() { return VulkanContext::s_VulkanContext; }

    VulkanContext::VulkanContext(VulkanSpec spec, Window* windowPtr) : m_WindowPtr(windowPtr), m_Spec(spec) {}

    ResultValueType<VulkanInstanceStatus> VulkanContext::CreateInstance()
    {
        LOG_INFO("Creating Vulkan Instance\n");
        LOG_INFO("    Application Name: %s\n", m_Spec.rendererSpec.AppName.data());
        LOG_INFO("    Engine Name: %s\n", ENGINE_INFO.Name.c_str());
        LOG_INFO("    Engine Version: %d\n", ENGINE_INFO.Version);

        uint32_t api_Version = VK_API_VERSION_1_3;
        LOG_INFO("    Vulkan API Version: %d\n", api_Version);

        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = m_Spec.rendererSpec.AppName.data();
        applicationInfo.applicationVersion = 1;
        applicationInfo.pEngineName = ENGINE_INFO.Name.c_str();
        applicationInfo.engineVersion = ENGINE_INFO.Version;
        applicationInfo.apiVersion = api_Version;

        bool availableDebugFlag = DebugMessanger::IsDebugExtensionAvailable();

        std::vector<const char*> extensions;

        LOG_INFO("Enabled Instance Extensions:\n");
        for (auto& extension: m_Spec.extensions)
        {
            extensions.push_back(extension.c_str());
            LOG_INFO("    %s\n", extension.c_str());
        }

        if (availableDebugFlag)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            LOG_INFO("    %s\n", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = extensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

        vkCreateInstance(&instanceCreateInfo, nullptr, &VulkanContext::Get()->m_Instance);

        return ResultValueType<VulkanInstanceStatus>(VulkanInstanceStatus::Created);
    }

    ResultValueType<VulkanPhysicalDeviceStatus> VulkanContext::SelectPhysicalDevice()
    {
        LOG_INFO("Found Devices:\n");
        uint32_t deviceCount{};
        vkEnumeratePhysicalDevices(VulkanContext::Get()->m_Instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> enumeratedDevices(deviceCount);
        vkEnumeratePhysicalDevices(VulkanContext::Get()->m_Instance, &deviceCount, enumeratedDevices.data());

        for (auto device: enumeratedDevices)
        {
            VkPhysicalDeviceProperties deviceProps{};
            vkGetPhysicalDeviceProperties(device, &deviceProps);

            LOG_INFO("    %s\n", deviceProps.deviceName);
            LOG_INFO("        Type: %d\n", deviceProps.deviceType);
            LOG_INFO("        Vendor: %d\n", deviceProps.vendorID);
            LOG_INFO("        Driver Version: %d\n", deviceProps.driverVersion);

            if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { m_PhysicalDevice = device; }
        }

        if (VK_NULL_HANDLE == m_PhysicalDevice)
        {
            if (enumeratedDevices.size() > 0)
            {
                m_PhysicalDevice = enumeratedDevices[0];

                LOG_INFO("Selected Physical Device!\n");
                return {VulkanPhysicalDeviceStatus::Selected};
            }
            else
            {
                LOG_ERROR("Physical Device Not Found!\n");
                return {VulkanPhysicalDeviceStatus::Not_Found};
            }
        }

        VkPhysicalDeviceProperties deviceProps{};
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProps);
        std::string selectedDevice = std::string(deviceProps.deviceName);
        LOG_INFO("Selected: %s\n", deviceProps.deviceName);

        return {VulkanPhysicalDeviceStatus::Selected};
    }

    ResultValueType<VulkanQueueFamilyStatus> VulkanContext::SelectQueueFamily()
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        //Find Graphics Queue Index
        auto propertyIterator =
                std::find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(),
                             [](VkQueueFamilyProperties const& qfp) { return qfp.queueFlags & VK_QUEUE_GRAPHICS_BIT; });
        size_t graphicsQueueFamilyIndex = std::distance(queueFamilyProperties.begin(), propertyIterator);
        if (graphicsQueueFamilyIndex >= queueFamilyProperties.size())
        {
            LOG_ERROR("Queues Not Found!\n");
            return {VulkanQueueFamilyStatus::Not_Found};
        }

        size_t presentQueueFamilyIndex;
        VkBool32 supported;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, graphicsQueueFamilyIndex, m_Surface, &supported);

        presentQueueFamilyIndex = supported ? graphicsQueueFamilyIndex : queueFamilyProperties.size();

        if (presentQueueFamilyIndex != queueFamilyProperties.size()) { return {VulkanQueueFamilyStatus::Found}; }

        // the graphicsQueueFamilyIndex doesn't support present -> look for an other family index that supports both
        // graphics and present
        for (size_t i = 0; i < queueFamilyProperties.size(); i++)
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &supported);
            if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && supported)
            {
                graphicsQueueFamilyIndex = i;
                presentQueueFamilyIndex = i;
                break;
            }
        }
        if (presentQueueFamilyIndex == queueFamilyProperties.size())
        {
            // there's nothing like a single family index that supports both graphics and present -> look for an other
            // family index that supports present
            for (size_t i = 0; i < queueFamilyProperties.size(); i++)
            {
                vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &supported);
                if (supported)
                {
                    presentQueueFamilyIndex = i;
                    break;
                }
            }
        }

        if (presentQueueFamilyIndex == queueFamilyProperties.size())
        {
            LOG_ERROR("Queues Not Found!\n");
            return {VulkanQueueFamilyStatus::Not_Found};
        }

        m_PresentQueueIndex = presentQueueFamilyIndex;
        m_GraphicsQueueIndex = graphicsQueueFamilyIndex;

        LOG_INFO("Selected Queues!\n");

        return {VulkanQueueFamilyStatus::Found};
    }

    ResultValueType<VulkanSwapchainStatus> VulkanContext::GetCapabilities()
    {
        LOG_INFO("Retrieving Capabilities...\n");
        uint32_t surfaceFormatCount = 0;
        auto surfaceFormatStatus =
                vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatCount, nullptr);
        m_SupportedFormats.resize(surfaceFormatCount);
        surfaceFormatStatus = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatCount,
                                                                   m_SupportedFormats.data());
        if (VK_SUCCESS != surfaceFormatStatus)
        {
            LOG_ERROR("Can Not Retrieve Surface Formats!\n");
            return {VulkanSwapchainStatus::Fail};
        }

        if (m_SupportedFormats.empty())
        {
            LOG_ERROR("Surface Formats Not Available!\n");
            return {VulkanSwapchainStatus::Fail};
        }

        m_Format = m_SupportedFormats[0].format;
        if (VK_FORMAT_UNDEFINED == m_SupportedFormats[0].format) { m_Format = VK_FORMAT_B8G8R8A8_UNORM; }

        auto surfaceCapabilitiesStatus =
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &m_SurfaceCapabilities);
        if (VK_SUCCESS != surfaceCapabilitiesStatus)
        {
            LOG_ERROR("Can Not Retrieve Surface Capabilities!\n");
            return {VulkanSwapchainStatus::Fail};
        }

        if (m_SurfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
        {
            m_SwapchainExtent.width = std::clamp(m_Spec.rendererSpec.width, m_SurfaceCapabilities.minImageExtent.width,
                                                 m_SurfaceCapabilities.maxImageExtent.width);
            m_SwapchainExtent.height =
                    std::clamp(m_Spec.rendererSpec.height, m_SurfaceCapabilities.minImageExtent.height,
                               m_SurfaceCapabilities.maxImageExtent.height);
        }
        else { m_SwapchainExtent = m_SurfaceCapabilities.currentExtent; }

        return {VulkanSwapchainStatus::Success};
    }

    ResultValueType<VulkanSwapchainStatus> VulkanContext::CreateSwapchain()
    {

        auto queueFamilyStatus = SelectQueueFamily();
        if (VulkanQueueFamilyStatus::Found != queueFamilyStatus)
        {
            LOG_ERROR("Swapchain Can Not Find Queue Family Indices!\n");
            return {VulkanSwapchainStatus::Fail};
        }

        auto capabilitiesStatus = GetCapabilities();
        if (VulkanSwapchainStatus::Success != capabilitiesStatus)
        {
            LOG_ERROR("Swapchain Can Not Get Capabiliteis!\n");
            return {VulkanSwapchainStatus::Fail};
        }

        VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

        VkSurfaceTransformFlagBitsKHR preTransform =
                (m_SurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
                        ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
                        : m_SurfaceCapabilities.currentTransform;

        VkCompositeAlphaFlagBitsKHR compositeAlpha =
                (m_SurfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
                        ? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
                : (m_SurfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
                        ? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
                : (m_SurfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
                        ? VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
                        : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        VkSwapchainCreateInfoKHR swapChainCreateInfo = {};

        swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainCreateInfo.surface = m_Surface;
        swapChainCreateInfo.minImageCount =
                std::clamp(3u, m_SurfaceCapabilities.minImageCount, m_SurfaceCapabilities.maxImageCount);
        swapChainCreateInfo.imageFormat = m_Format;
        swapChainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        swapChainCreateInfo.imageExtent = m_SwapchainExtent;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapChainCreateInfo.preTransform = preTransform;
        swapChainCreateInfo.compositeAlpha = compositeAlpha;
        swapChainCreateInfo.presentMode = swapchainPresentMode;
        swapChainCreateInfo.clipped = VK_TRUE;
        swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        uint32_t queueFamilyIndices[2] = {static_cast<uint32_t>(m_GraphicsQueueIndex),
                                          static_cast<uint32_t>(m_PresentQueueIndex)};
        if (m_GraphicsQueueIndex != m_PresentQueueIndex)
        {
            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapChainCreateInfo.queueFamilyIndexCount = 2;
            swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        }

        auto swapchainStatus = vkCreateSwapchainKHR(m_Device, &swapChainCreateInfo, nullptr, &m_Swapchain);
        if (VK_SUCCESS != swapchainStatus)
        {
            LOG_ERROR("Can Not Create Swapchain!\n");
            return {VulkanSwapchainStatus::Fail};
        }

        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, nullptr);
        m_Images.resize(imageCount);
        auto swapchainImagesStatus = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, m_Images.data());
        if (VK_SUCCESS != swapchainImagesStatus)
        {
            LOG_ERROR("Can Not Retrieve Swapchain Images!\n");
            return {VulkanSwapchainStatus::Fail};
        }

        for (uint32_t index = 0; index < m_Images.size(); index++)
        {
            VkImageViewCreateInfo imageviewCreateInfo = {};
            imageviewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageviewCreateInfo.image = m_Images[index];
            imageviewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageviewCreateInfo.format = m_Format;
            imageviewCreateInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

            auto viewStatus = vkCreateImageView(m_Device, &imageviewCreateInfo, nullptr, &m_ImageViews[index]);
            if (VK_SUCCESS != viewStatus)
            {
                LOG_ERROR("Can Not Create Image Views!\n");
                return {VulkanSwapchainStatus::Fail};
            }
        }

        LOG_INFO("Created Swapchain!\n");
        return {VulkanSwapchainStatus::Created};
    }

    ResultValueType<VulkanDeviceStatus> VulkanContext::CreateDevice()
    {
        uint32_t availableExtensionCount = 0;
        auto result =
                vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &availableExtensionCount, nullptr);
        if (VK_SUCCESS != result)
        {
            LOG_ERROR("Can Not Retrieve Available Device Extensions!\n");
            return {VulkanDeviceStatus::Fail};
        }
        std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
        result = vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &availableExtensionCount,
                                                      availableExtensions.data());
        if (VK_SUCCESS != result)
        {
            LOG_ERROR("Can Not Retrieve Available Device Extensions!\n");
            return {VulkanDeviceStatus::Fail};
        }
        auto propertyIterator = std::find_if(
                availableExtensions.begin(), availableExtensions.end(), [](VkExtensionProperties const& properties) {
                    return std::string(properties.extensionName) == std::string("VK_KHR_swapchain");
                });
        if (availableExtensions.end() == propertyIterator)
        {
            LOG_ERROR("Device Swapchain Extension Not Available!\n");
            return {VulkanDeviceStatus::Fail};
        }

        float queuePriority = 0.0f;
        VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
        deviceQueueCreateInfo.queueFamilyIndex = m_GraphicsQueueIndex;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

        const std::vector<const char*> deviceExtensions = {"VK_KHR_swapchain"};
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        createInfo.enabledExtensionCount = 1;
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        auto deviceStatus = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);

        if (VK_SUCCESS != deviceStatus)
        {
            LOG_ERROR("Can Not Create Logical Device!\n");
            return {VulkanDeviceStatus::Fail};
        }

        LOG_INFO("Logical Device Created\n");
        return {VulkanDeviceStatus::Created};
    }

}// namespace Engine