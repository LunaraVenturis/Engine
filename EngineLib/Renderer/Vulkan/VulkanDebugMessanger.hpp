#pragma once
#include <vulkan/vulkan.h>

namespace LunaraEngine
{
    class VulkanDebugMessanger
    {
    public:
        VulkanDebugMessanger() = default;
        ~VulkanDebugMessanger() = default;
        VulkanDebugMessanger(const VulkanDebugMessanger& other) = delete;
        VulkanDebugMessanger& operator=(const VulkanDebugMessanger&) = delete;
        VulkanDebugMessanger(VulkanDebugMessanger&& other) = delete;
        VulkanDebugMessanger& operator=(VulkanDebugMessanger&& other) = delete;

    public:
        void Create(VkInstance instance);
        void Destroy(VkInstance instance);

    private:
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void* pUserData);
        static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                                  const VkAllocationCallbacks* pAllocator);

        static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                     const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                     const VkAllocationCallbacks* pAllocator,
                                                     VkDebugUtilsMessengerEXT* pDebugMessenger);

    private:
        VkDebugUtilsMessengerEXT m_DebugMessenger;
    };
}// namespace LunaraEngine