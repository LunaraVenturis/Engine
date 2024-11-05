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
#include <types.hpp>
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
    class Window : public RefCounted
    {
    public:
        virtual ~Window();
        virtual ResultValueType<WindowStatus> Init() = 0;
        virtual ResultValueType<WindowStatus> CreateSurface(VkInstance instance) = 0;
        virtual ResultValueType<WindowStatus> DestroySurface(VkInstance instance) = 0;
        virtual ResultValue<WindowStatus, VkSurfaceKHR> GetSurface() const = 0;
        virtual ResultValue<bool, std::vector<std::string>> GetRequiredExtensions() = 0;

    public:
        static void PollEvents();
        static i32 ShouldClose() { return s_WindowShouldClose; }
        static ResultValue<WindowStatus, Window*> Create(RendererSpec& rendererSpec);
        static ResultValueType<WindowStatus> Destroy(Window* window);

    protected:
        inline static i32 s_WindowShouldClose;
    private:
        RendererSpec m_RendererSpec;
        VkSurfaceKHR m_Surface;

    };
}// namespace Engine