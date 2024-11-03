/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 

 * @section DESCRIPTION
 * 
 * Window class implementation
 */

#include <Core/Log.hpp>
#include "Window.hpp"

namespace Engine
{

    ResultValueType<WindowStatus> Window::Init(RendererSpec& rendererSpec)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_WindowPtr = glfwCreateWindow(rendererSpec.width, rendererSpec.height, rendererSpec.AppName.data(), nullptr,
                                       nullptr);


        if (!glfwVulkanSupported())
        {
            LOG_ERROR("Vulkan API is not supported\n");
            return ResultValueType{WindowStatus::Fail};
        }
        else { LOG_INFO("Found support for Vulkan API\n"); }

        LOG_INFO("Created Window\n");
        return ResultValueType{WindowStatus::Created};
    }

    ResultValueType<WindowStatus> Window::CreateSurface(VkInstance instance)
    {
        WindowStatus status = WindowStatus::Fail;

#ifdef VK_USE_PLATFORM_WIN32_KHR
        VkWin32SurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = glfwGetWin32Window(m_WindowPtr);
        createInfo.hinstance = GetModuleHandle(NULL);
        auto resultValue = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &m_Surface);
        if (VK_SUCCESS == resultValue) { status = WindowStatus::Surface_Created; }
#else
        LOG_ERROR("Non Windows platforms not supported at the moment!\n");
#endif

        return ResultValueType{status};
    }

    ResultValueType<WindowStatus> Window::DestroySurface(VkInstance instance)
    {
        vkDestroySurfaceKHR(instance, m_Surface, nullptr);

        return ResultValueType{WindowStatus::Surface_Destroyed};
    }

    ResultValue<WindowStatus, VkSurfaceKHR> Window::GetSurface()
    {
        if (VK_NULL_HANDLE == (VkSurfaceKHR) m_Surface)
        {
            return ResultValue<WindowStatus, VkSurfaceKHR>{WindowStatus::Fail, VK_NULL_HANDLE};
        }

        return ResultValue{WindowStatus::Success, m_Surface};
    }

    Window::~Window() { LOG_INFO("Destroyed Window\n"); }

    ResultValue<WindowStatus, Window*> Window::Create(RendererSpec& rendererSpec)
    {
        auto window = Allocator::Allocate<Window>();
        Window* window = new Window();
        auto result = window->Init(rendererSpec);

        return ResultValue<WindowStatus, Window*>{result, window};
    }

    ResultValueType<WindowStatus> Window::Destroy(Window* window)
    {
        if (nullptr == window) { return ResultValueType{WindowStatus::Not_Initialized}; }
        else
        {
            if (nullptr != window->m_WindowPtr) { glfwDestroyWindow(window->m_WindowPtr); }
            delete window;
            return ResultValueType{WindowStatus::Destroyed};
        }
    }

    ResultValue<bool, std::vector<std::string>> Window::GetRequiredExtensions()
    {
        uint32_t count;
        const char** extensions = glfwGetRequiredInstanceExtensions(&count);

        std::vector<std::string> output;

        LOG_INFO("Required instance extensions:\n");
        for (uint32_t i = 0; i < count; i++)
        {
            LOG_INFO("    %s\n", extensions[i]);
            output.push_back(std::string(extensions[i]));
        }

        return ResultValue{true, output};
    }

}// namespace Engine