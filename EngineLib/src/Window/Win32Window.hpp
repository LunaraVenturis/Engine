/**
 * @file
 * @author Neyko Naydenov ( neyko641@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * Win32 implementation of the Window class.
 */


#include <Platform/WindowInstance.hpp>
#include <types.hpp>
#include <optional>
#include "Renderer/Window.hpp"

namespace Engine
{

    class Win32Window: public Window
    {
    public:
        Win32Window(const RendererSpec& spec) noexcept;
        ~Win32Window();
        Win32Window(const Win32Window&) = delete;
        Win32Window& operator=(const Win32Window&) = delete;

    public:
        ResultValueType<WindowStatus> Init() override;
        ResultValueType<WindowStatus> CreateSurface(VkInstance instance) override;
        ResultValueType<WindowStatus> DestroySurface(VkInstance instance) override;
        ResultValue<WindowStatus, VkSurfaceKHR> GetSurface() const override;
        ResultValue<bool, std::vector<std::string>> GetRequiredExtensions();
    public:
        static LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        static LRESULT CALLBACK HandleMessageThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        static void ProcessMessages();

    public:
        //  Keyboard kbd;
    protected:
        RendererSpec p_RenderSpec;
        VkSurfaceKHR m_Surface;

    private:
        HWND m_hWnd;
        HINSTANCE m_hInstance;
    };
}// namespace Engine