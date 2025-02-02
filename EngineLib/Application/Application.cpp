#include "Application.hpp"
#include "Engine.hpp"
#include <Audio/AudioManager.hpp>

namespace LunaraEngine
{
    // inline static Application s_Instance;

    ApplicationResult Application::Create(std::string_view name, uint32_t width, uint32_t height)
    {
        auto renderer_result = Renderer::Init(name, width, height);
        if (renderer_result != Renderer_Result_Success) { return ApplicationResult_Fail; }

        auto audio_manager_result = AudioManager::Init();
        if (audio_manager_result != AudioManager_Result_Success) { return ApplicationResult_Fail; }

        return ApplicationResult_Success;
    }

    void Application::Run()
    {
        LayerStack::InitLayers();
        Event event;
        while (true)
        {
            if (PollEvents(&event))
            {
                if (event.type == EVENT_QUIT) { break; }
            }

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