#include "SandboxLayer.hpp"
#include "Engine.hpp"

void SandboxLayer::Init()
{
    m_Window = LunaraEngine::Renderer::GetWindow();
    LunaraEngine::AudioManager::LoadAudio("AudioTest", "Assets/Audio/AudioTest.wav");
}

void SandboxLayer::OnUpdate(float dt)
{
    (void) dt;
    using namespace LunaraEngine;

    Renderer::BeginRenderPass();

    Renderer::Clear(Color4{0.0f, 0.0f, 0.0f, 1.0f});

    Renderer::DrawQuad(FRect{0.0f, 0.0f, 100.0f, 100.0f}, Color4{1.0f, 0.0f, 0.0f, 1.0f});

    Renderer::EndRenderPass();

    //Play audio test
    if (!LunaraEngine::AudioManager::IsAudioPlaying("AudioTest"))
    {
        LunaraEngine::AudioManager::PlayAudio("AudioTest");
    }

    //Flush command buffer for drawing
    Renderer::Flush();
}
