#include "Application.hpp"
#include <LunaraEngine/Engine.hpp>
#include <LunaraEngine/Core/Timer.hpp>

namespace LunaraEngine
{
    ApplicationResult Application::Create(ApplicationConfig&& config)
    {

        if (!ValidateConfig(std::forward<ApplicationConfig>(config))) { return ApplicationResult_Fail; }

        s_Config = std::move(config);

        auto renderer_result = Renderer::Init(s_Config.windowName, s_Config.initialWidth, s_Config.initialHeight);
        if (renderer_result != LunaraEngine::RendererResultType::Renderer_Result_Success)
        {
            return ApplicationResult_Fail;
        }

        auto audio_manager_result = AudioManager::Init();
        if (audio_manager_result != AudioManager_Result_Success) { return ApplicationResult_Fail; }

        return ApplicationResult_Success;
    }

    void Application::Run()
    {
        LayerStack::InitLayers(s_Config);
        Event event{};
        double dt = 0.0f;
        Timer timer;
        while (true)
        {
            if (PollEvents(&event))
            {
                if (event.type == EVENT_QUIT) { break; }

                LayerStack::OnEvent(&event);
            }
            Renderer::BeginFrame();
            LayerStack::Begin();
            LayerStack::OnUpdate((float) dt);
            LayerStack::OnImGuiDraw();
            LayerStack::End();

            //Present to screen
            Renderer::Present();

            dt = timer.Elapsed();
            timer.Reset();
        }
    }

    void Application::Close()
    {
        LayerStack::DestroyLayers();
        AudioManager::Destroy();
        Renderer::Destroy();
    }

    bool Application::ValidateConfig(ApplicationConfig&& config)
    {
        LOG_INFO("Validating application config...");
        LOG_DEBUG("Working directory: %s", config.workingDirectory.c_str());
        LOG_DEBUG("Assets directory: %s", config.assetsDirectory.c_str());
        LOG_DEBUG("Shaders directory: %s", config.shadersDirectory.c_str());
        LOG_DEBUG("Textures directory: %s", config.texturesDirectory.c_str());
        LOG_DEBUG("Window name: %s", config.windowName.data());
        LOG_DEBUG("\tInitial width: %d", config.initialWidth);
        LOG_DEBUG("\tInitial height: %d", config.initialHeight);

        if (config.workingDirectory.empty()) { return false; }
        if (config.assetsDirectory.empty()) { return false; }
        if (config.shadersDirectory.empty()) { return false; }
        if (config.texturesDirectory.empty()) { return false; }
        if (config.windowName.empty()) { return false; }
        if (config.initialWidth == 0) { return false; }
        if (config.initialHeight == 0) { return false; }

        LOG_INFO("Application config is valid");
        return true;
    }

}// namespace LunaraEngine