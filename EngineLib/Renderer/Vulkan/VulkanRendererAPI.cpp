/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com ) 
 * @coauthor Neyko Naydenov ( neyko641@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section LICENSE
 * MIT License
 * 
 * Copyright (c) 2025 Krusto, Neyko
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @section DESCRIPTION
 * 
 * Renderer definitions
 */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "VulkanRendererAPI.hpp"
#include "Renderer/Window.hpp"
#include "SDL3/SDL_vulkan.h"
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <array>

namespace LunaraEngine
{
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> computeFamily;
        //std::optional<uint32_t> presentFamily;

        bool isComplete() { return graphicsFamily.has_value() && computeFamily.has_value();}
    };
    
    void VulkanRendererAPI::CreateWindow()
    {
        m_RendererData->window = new Window;
        m_RendererData->window->name = "Test";
        m_RendererData->window->data = (void*)SDL_CreateWindow(m_RendererData->window->name, 1280, 720, SDL_WINDOW_VULKAN);
        if(m_RendererData->window->data == nullptr)
        {
            throw std::runtime_error("Couldn't create Window");
        }
    }
    void VulkanRendererAPI::Init()
    {
        m_RendererData = std::make_unique<RendererDataType>();
        CreateWindow();
        CreateInstance();
        PickPhysicalDevice();
        CreateLogicalDevice();
    }

    void VulkanRendererAPI::Destroy() { CleanUpVulkan(); }

    void VulkanRendererAPI::CreateInstance()
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

        std::vector<const char*> names;
        GetPlatformExtensions(names);
        

#ifdef _DEBUG
        createInfo.enabledLayerCount = 1;
        std::array<const char*, 1> layers = {"VK_LAYER_KHRONOS_validation"};
        createInfo.ppEnabledLayerNames = layers.data();
        names.push_back("VK_EXT_debug_utils");
        names.push_back("VK_EXT_debug_report");
#else
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
#endif
        createInfo.enabledExtensionCount = static_cast<uint32_t>(names.size());
        createInfo.ppEnabledExtensionNames = names.data();

        if (vkCreateInstance(&createInfo, nullptr, &m_RendererData->instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create instance!");
        }
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = VulkanRendererAPI::DebugCallback;
        debugCreateInfo.pUserData = nullptr;// Optional
        
        CreateDebugUtilsMessengerEXT(m_RendererData->instance, &debugCreateInfo, nullptr, &m_RendererData->debug);
    }

    void VulkanRendererAPI::GetPlatformExtensions(std::vector<const char*>& extensions)
    {
        uint32_t extensionCount = 0;
       const char* const * neededExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
      while(*neededExtensions != nullptr)
      {
          extensions.emplace_back(*neededExtensions);
          neededExtensions++;
      }
      /* this will get refactored once we agree upon how to get extensions.
#ifdef _WIN32
        extensions.emplace_back("VK_KHR_win32_surface");
#elif linux
        extensions.emplace_back("VK_KHR_xlib_surface");
#endif
*/
    }

    void VulkanRendererAPI::CreateDevice() { throw std::runtime_error("Not implemented"); }

    void VulkanRendererAPI::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(m_RendererData->physicalDevice);

        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.has_value();
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;

        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;

        if (vkCreateDevice(m_RendererData->physicalDevice, &createInfo, nullptr, &m_RendererData->device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }
        vkGetDeviceQueue(m_RendererData->device, indices.graphicsFamily.value(), 0, &m_RendererData->gfxQueue);

    }

    void VulkanRendererAPI::PickPhysicalDevice()
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
                SetPhysicalDevice(device);
                break;
            }
        }
        
        if (m_RendererData->physicalDevice == VK_NULL_HANDLE) { throw std::runtime_error("failed to find a suitable GPU!"); }
    }

    bool VulkanRendererAPI::IsDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indicies = FindQueueFamilies(device);

        return indicies.isComplete();
    }

    QueueFamilyIndices VulkanRendererAPI::FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        //VkBool32 presentSupport = false;
        //vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);


        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily: queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) { indices.graphicsFamily = i; }
            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) { indices.computeFamily = i; }
            if (indices.isComplete()) { break; }

            i++;
        }

        return indices;
    }

    void VulkanRendererAPI::CleanUpVulkan() 
    { 
        vkDestroyInstance(m_RendererData->instance, nullptr);
        vkDestroyDevice(m_RendererData->device, nullptr);
        SDL_DestroyWindow((SDL_Window*)m_RendererData->window->data);
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRendererAPI::DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        (void) messageType;
        (void) pUserData;
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) printf("%s\n", pCallbackData->pMessage);
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) printf("%s\n", pCallbackData->pMessage);

        return VK_FALSE;
    }

    void VulkanRendererAPI::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                                          const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) { func(instance, debugMessenger, pAllocator); }
    }

    VkResult VulkanRendererAPI::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                             const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                             const VkAllocationCallbacks* pAllocator,
                                                             VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func =
                (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) { return func(instance, pCreateInfo, pAllocator, pDebugMessenger); }
        else { return VK_ERROR_EXTENSION_NOT_PRESENT; }
    }


}// namespace LunaraEngine