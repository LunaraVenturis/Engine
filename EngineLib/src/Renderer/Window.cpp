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
#include "Window/Win32Window.hpp"
namespace Engine
{


    Window::~Window() { LOG_INFO("Destroyed Window\n"); }

    ResultValue<WindowStatus, Window*> Window::Create(RendererSpec& rendererSpec)
    {
        Window* window = new Win32Window(rendererSpec);
        auto result = window->Init();

        return ResultValue<WindowStatus, Window*>{result, window};
    }

    ResultValueType<WindowStatus> Window::Destroy(Window* window)
    {
        if (nullptr == window) { return ResultValueType{WindowStatus::Not_Initialized}; }
        else
        {
            delete window;
            return ResultValueType{WindowStatus::Destroyed};
        }
    }



}// namespace Engine