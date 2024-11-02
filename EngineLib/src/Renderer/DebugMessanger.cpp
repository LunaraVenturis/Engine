/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 *
 * @section DESCRIPTION
 * 
 * Debug Messanger class implementation
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "DebugMessanger.hpp"
#include <Core/Log.hpp>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include <sstream>

PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;
Engine::DebugMessanger* Engine::DebugMessanger::s_DebugMessanger = {};

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator,
                                                              VkDebugUtilsMessengerEXT* pMessenger)
{
    return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger,
                                                           VkAllocationCallbacks const* pAllocator)
{
    return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageFunc(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
                                                void* /*pUserData*/)
{
    std::ostringstream message;

    message << messageSeverity << ": " << messageTypes << ":\n";
    message << std::string("\t") << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
    message << std::string("\t") << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
    message << std::string("\t") << "message         = <" << pCallbackData->pMessage << ">\n";
    if (0 < pCallbackData->queueLabelCount)
    {
        message << std::string("\t") << "Queue Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++)
        {
            message << std::string("\t\t") << "labelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < pCallbackData->cmdBufLabelCount)
    {
        message << std::string("\t") << "CommandBuffer Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
        {
            message << std::string("\t\t") << "labelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < pCallbackData->objectCount)
    {
        message << std::string("\t") << "Objects:\n";
        for (uint32_t i = 0; i < pCallbackData->objectCount; i++)
        {
            message << std::string("\t\t") << "Object " << i << "\n";
            message << std::string("\t\t\t") << "objectType   = " << pCallbackData->pObjects[i].objectType << "\n";
            message << std::string("\t\t\t") << "objectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
            if (pCallbackData->pObjects[i].pObjectName)
            {
                message << std::string("\t\t\t") << "objectName   = <" << pCallbackData->pObjects[i].pObjectName
                        << ">\n";
            }
        }
    }

    switch (messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_INFO(&message.str()[0]);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(&message.str()[0]);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARNING(&message.str()[0]);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR(&message.str()[0]);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            LOG_ERROR(&message.str()[0]);
            break;
    }

    return false;
}

namespace Engine
{
    ResultValueType<DebugMessangerStatus> DebugMessanger::Create(VkInstance& instance)
    {
        ResultValueType<DebugMessangerStatus> result;
#if !defined(NDEBUG)
        DebugMessanger::s_DebugMessanger = new DebugMessanger();
        result = DebugMessanger::Get()->Init(instance);
#else
        result = {DebugMessangerStatus::Created};
#endif

        return result;
    }

    void DebugMessanger::Destroy(VkInstance& instance)
    {
        if (DebugMessanger::Get())
        {
            vkDestroyDebugUtilsMessengerEXT(instance, DebugMessanger::Get()->m_VkDebugMessanger, nullptr);
            delete DebugMessanger::Get();
            LOG_INFO("Debug Messanger Destroyed\n");
        }
    }

    DebugMessanger* DebugMessanger::Get() { return DebugMessanger::s_DebugMessanger; }

    bool DebugMessanger::IsDebugExtensionAvailable()
    {
        uint32_t propertyCount{};
        vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);
        std::vector<VkExtensionProperties> props(propertyCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, props.data());

        auto propertyIterator = std::find_if(props.begin(), props.end(), [](VkExtensionProperties const& ep) {
            return strcmp(ep.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0;
        });
        if (propertyIterator == props.end())
        {
            LOG_ERROR("Something went very wrong, cannot find " + std::string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) +
                      " extension\n");
            return false;
        }

        return true;
    }
}// namespace Engine

namespace Engine
{
    ResultValueType<DebugMessangerStatus> DebugMessanger::Init(VkInstance& instance)
    {
        pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        if (!pfnVkCreateDebugUtilsMessengerEXT)
        {
            LOG_INFO("GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.\n");
            return ResultValueType{DebugMessangerStatus::Fail};
        }


        pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (!pfnVkDestroyDebugUtilsMessengerEXT)
        {
            LOG_INFO("GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.\n");
            return ResultValueType{DebugMessangerStatus::Fail};
        }

        VkDebugUtilsMessengerCreateInfoEXT debugMessageFuncCreateInfo = {};
        debugMessageFuncCreateInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        debugMessageFuncCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessageFuncCreateInfo.pfnUserCallback = &debugMessageFunc;

        vkCreateDebugUtilsMessengerEXT(instance, &debugMessageFuncCreateInfo, nullptr, &m_VkDebugMessanger);

        LOG_INFO("Debug Messanger Created\n");
        return ResultValueType{DebugMessangerStatus::Created};
    }


}// namespace Engine