#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * Vulkan Debug Messanger class definitions
 */


#include <Core/Result.hpp>

#include <vulkan/vulkan.hpp>

namespace Engine
{
    enum class DebugMessangerStatus
    {
        Fail,
        Created,
        Destroyed
    };
}

namespace Engine
{
    class DebugMessanger
    {
    public:
        DebugMessanger() = default;
        ~DebugMessanger() = default;

    public:
        static ResultValueType<DebugMessangerStatus> Create(VkInstance& instance);
        static void Destroy(VkInstance& instance);
        static DebugMessanger* Get();
        static bool IsDebugExtensionAvailable();

    public:
        ResultValueType<DebugMessangerStatus> Init(VkInstance& instance);

    private:
        VkDebugUtilsMessengerEXT m_VkDebugMessanger;

    public:
        static DebugMessanger* s_DebugMessanger;
    };
}// namespace Engine