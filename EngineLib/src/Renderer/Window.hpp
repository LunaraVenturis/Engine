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
 * Window class definition
 */

#include <vector>

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <Core/Result.hpp>
#include <Core/Ref.hpp>
#include <Renderer/RendererSpec.hpp>


enum class WindowStatus
{
    Success,
    Fail,
    Created,
    Initialized,
    Not_Initialized,
    Destroyed,
    Surface_Created,
    Surface_Destroyed
};

namespace Engine
{

    class Window: public RefCounted
    {
    public:
        Window() = default;
        ~Window();

    public:
        ResultValueType<WindowStatus> Init(RendererSpec& rendererSpec);
        ResultValueType<WindowStatus> CreateSurface(VkInstance instance);
        ResultValueType<WindowStatus> DestroySurface(VkInstance instance);
        ResultValue<WindowStatus, VkSurfaceKHR> GetSurface();
        
    public:
        static ResultValue<WindowStatus, Window*> Create(RendererSpec& rendererSpec);
        static ResultValueType<WindowStatus> Destroy(Window* window);

        static ResultValue<bool, std::vector<std::string>> GetRequiredExtensions();

    private:
        RendererSpec m_RendererSpec;
        GLFWwindow* m_WindowPtr;
        VkSurfaceKHR m_Surface;
    };
}// namespace Engine