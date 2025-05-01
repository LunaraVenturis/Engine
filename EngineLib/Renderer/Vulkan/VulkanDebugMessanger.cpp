#include "VulkanDebugMessanger.hpp"
#include <vulkan/vulkan.h>
#include <Core/Log.h>

namespace LunaraEngine
{
    void VulkanDebugMessanger::Create(VkInstance instance)
    {
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = VulkanDebugMessanger::DebugCallback;
        debugCreateInfo.pUserData = nullptr;// Optional

        CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &m_DebugMessenger);
    }

    void VulkanDebugMessanger::Destroy(VkInstance instance)
    {
        DestroyDebugUtilsMessengerEXT(instance, m_DebugMessenger, nullptr);
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugMessanger::DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        (void) messageType;
        (void) pUserData;
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            LOG_ERROR("%s\n", pCallbackData->pMessage);
        }
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            LOG_WARNING("%s\n", pCallbackData->pMessage);
        }
        return VK_FALSE;
    }

    void VulkanDebugMessanger::DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                                             VkDebugUtilsMessengerEXT debugMessenger,
                                                             const VkAllocationCallbacks* pAllocator)
    {
        const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr) { func(instance, debugMessenger, pAllocator); }
    }

    VkResult VulkanDebugMessanger::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                                const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                                const VkAllocationCallbacks* pAllocator,
                                                                VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        if (func != nullptr) { return func(instance, pCreateInfo, pAllocator, pDebugMessenger); }
        else { return VK_ERROR_EXTENSION_NOT_PRESENT; }
    }
}// namespace LunaraEngine