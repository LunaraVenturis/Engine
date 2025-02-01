#include <Engine.hpp>

int main()
{
    auto result = RendererInit("Engine", 1280, 720);
    if (result != Renderer_Result_Success) { return 1; }
    Event event;
    while (true)
    {
        if (PollEvents(&event))
        {
            if (event.type == EVENT_QUIT) { break; }
        }

        //Send quad draw command
        RendererCommandDrawQuad quad;
        quad.x = 1280 - 16;
        quad.y = 720 / 2;
        quad.width = 16;
        quad.height = 16;
        quad.r = 255;
        quad.g = 0;
        quad.b = 0;
        quad.a = 255;
        RendererCmdDrawQuad(&quad);

        //Flush command buffer for drawing
        RendererCmdFlush();

        //Present to screen
        RendererPresent();
    }
    RendererDestroy();
    return 0;
}