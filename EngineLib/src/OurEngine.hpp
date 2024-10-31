#pragma once
#include <iostream>
#include "Window/Window.hpp"
#include <Core/Core.hpp>
#include <Core/Timer.hpp>

namespace OurEngine
{
    inline static void InitEngine() 
    { 
        Window window(800, 600, "Engine");
    }
    
}// namespace OurEngine