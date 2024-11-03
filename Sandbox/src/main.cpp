#include "SandboxLayer.hpp"
#include <Engine.hpp>

int main()
{
    Engine::Application::Init(Engine::ApplicationSpec{.ApplicationName = "Sandbox Application",
                                                      .WorkingDirectory = std::filesystem::current_path(),
                                                      .StartupWidth = 1280,
                                                      .StartupHeight = 720});
    Engine::Application::AddLayer<SandboxLayer>();

    Engine::Application::Run();

    Engine::Application::Destroy();
    return 0;
}