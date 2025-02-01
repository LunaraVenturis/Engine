#include <Engine.hpp>

int main()
{
    using namespace LunaraEngine;
    auto result = Renderer::Init("Engine", 1280, 720);
    if (result != Renderer_Result_Success) { return 1; }
    Event event;
    while (true)
    {
        if (PollEvents(&event))
        {
            if (event.type == EVENT_QUIT) { break; }
        }

        //Send quad draw command
        Renderer::DrawQuad({1280 - 16, 720 / 2, 16, 16}, {255, 0, 0, 255});

        //Flush command buffer for drawing
        Renderer::Flush();

        //Present to screen
        Renderer::Present();
    }
    Renderer::Destroy();
    return 0;
}