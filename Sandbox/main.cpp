#include <Engine.hpp>
#include "SandboxLayer.hpp"

int main()
{
    
    LunaraEngine::LayerStack::PushLayer<SandboxLayer>("Sandbox");
    LunaraEngine::Application::Create("Sandbox", 1280, 720);
    LunaraEngine::Application::Run();
    LunaraEngine::Application::Close();

    return 0;
}