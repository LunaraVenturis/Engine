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

        RendererPresent();
    }
    RendererDestroy();
    return 0;
}