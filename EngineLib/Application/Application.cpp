#include "Application.hpp"
#include "Engine.hpp"
#include <Core/Timer.hpp>

namespace LunaraEngine
{
    ApplicationResult Application::Create(std::filesystem::path workingDirectory, std::string_view name, uint32_t width,
                                          uint32_t height)
    {
        m_WorkingDirectory = workingDirectory;

        LOG_INFO("Creating application: %s", name.data());
        LOG_INFO("Working directory: %s", m_WorkingDirectory.c_str());
        auto renderer_result = Renderer::Init(name, width, height);
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
        LayerStack::InitLayers(m_WorkingDirectory);
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

}// namespace LunaraEngine