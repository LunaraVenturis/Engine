#include <Engine.hpp>
#include "SandboxLayer.hpp"

int main()
{

    LunaraEngine::LayerStack::PushLayer<SandboxLayer>("Sandbox");
    LunaraEngine::Application::Create(std::filesystem::current_path(), "Sandbox", 1280, 720);
    LunaraEngine::Application::Run();
    LunaraEngine::Application::Close();

    return 0;
}