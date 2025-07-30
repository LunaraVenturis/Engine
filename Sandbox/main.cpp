#include <LunaraEngine/Engine.hpp>
#include "SandboxLayer.hpp"

int main()
{
    LunaraEngine::Application::Create(LunaraEngine::ApplicationConfig{
            .workingDirectory = std::filesystem::current_path(),
            .assetsDirectory = std::filesystem::current_path() / "Assets",
            .shadersDirectory = std::filesystem::current_path() / "Assets" / "Shaders/bin",
            .texturesDirectory = std::filesystem::current_path() / "Assets" / "Textures",
            .windowName = "Example Game",
            .initialWidth = 1280,
            .initialHeight = 720,
    });
    LunaraEngine::LayerStack::PushLayer<SandboxLayer>("Sandbox");
    LunaraEngine::Application::Run();
    LunaraEngine::Application::Close();

    return 0;
}