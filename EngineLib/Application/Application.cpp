#include "Application.hpp"
#include "Engine.hpp"
#include <Core/Timestep.h>

namespace LunaraEngine
{
    ApplicationResult Application::Create(std::filesystem::path workingDirectory, std::string_view name, uint32_t width,
                                          uint32_t height)
    {
        m_WorkingDirectory = workingDirectory;
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
            Timer_Start(&timer);
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

            Timer_End(&timer);
            dt = timer.t;
        }
    }

    void Application::Close()
    {
        LayerStack::DestroyLayers();
        AudioManager::Destroy();
        Renderer::Destroy();
    }

}// namespace LunaraEngine