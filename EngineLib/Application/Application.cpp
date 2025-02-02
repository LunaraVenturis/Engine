#include "Application.hpp"
#include "Engine.hpp"

namespace LunaraEngine
{
    // inline static Application s_Instance;

    ApplicationResult Application::Create(std::string_view name, uint32_t width, uint32_t height)
    {
        auto result = Renderer::Init(name, width, height);
        if (result != Renderer_Result_Success) { return ApplicationResult_Fail; }

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
        Renderer::Destroy();
    }

}// namespace LunaraEngine