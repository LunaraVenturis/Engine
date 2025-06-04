#include "Application.hpp"
#include "Engine.hpp"

namespace LunaraEngine
{
    // inline static Application s_Instance;

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
        Event event;
        while (true)
        {

            if (PollEvents(&event))
            {
                if (event.type == EVENT_QUIT) { break; }

                LayerStack::OnEvent(&event);
            }
            Renderer::BeginFrame();
            LayerStack::Begin();
            LayerStack::OnUpdate(1.0f);
            LayerStack::OnImGuiDraw();
            LayerStack::End();

            //Present to screen
            Renderer::Present();
        }
    }

    void Application::Close()
    {
        LayerStack::DestroyLayers();
        AudioManager::Destroy();
        Renderer::Destroy();
    }

}// namespace LunaraEngine