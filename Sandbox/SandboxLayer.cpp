#include "SandboxLayer.hpp"
#include "Engine.hpp"

void SandboxLayer::OnUpdate(float dt)
{
    (void) dt;
    using namespace LunaraEngine;

    Renderer::Clear(Color4{0.0f, 0.0f, 0.0f, 1.0f});
    Renderer::DrawQuad(FRect{0.0f, 0.0f, 100.0f, 100.0f}, COLOR_RED);
}