#include "SandboxLayer.hpp"
#include "Engine.hpp"

void SandboxLayer::Init()
{
    m_Window = LunaraEngine::Renderer::GetWindow();
    LunaraEngine::AudioManager::LoadAudio("AudioTest", "Assets/Audio/AudioTest.wav");
    auto result = LunaraEngine::LoadFont("Assets/Fonts/joystixmonospace.ttf", 24, &m_Font);
    if (result != LunaraEngine::FontResultType::FONT_RESULT_SUCCESS) { exit(-6); }
}

void SandboxLayer::OnUpdate(float dt)
{
    (void) dt;
    using namespace LunaraEngine;

    Renderer::BeginRenderPass();

    Renderer::Clear(Color4{0.0f, 0.0f, 0.0f, 1.0f});

    Renderer::DrawQuad(FRect{300.0f, 300.0f, 100.0f, 100.0f}, Color4{1.0f, 0.0f, 0.0f, 1.0f});

    size_t length = (size_t) snprintf(NULL, 0, "%u, %u", x, y);
    text[length] = '\0';
    snprintf(text, length + 1, "%d, %d", x, y);

    std::string_view t = std::string_view(text, length);
    Renderer::DrawText(t, &m_Font, 100.0f, 100.0f, Color4{1.0f, 1.0f, 1.0f, 1.0f});

    Renderer::EndRenderPass();

    //Play audio test
    if (!LunaraEngine::AudioManager::IsAudioPlaying("AudioTest"))
    {
        LunaraEngine::AudioManager::PlayAudio("AudioTest");
    }

    //Flush command buffer for drawing
    Renderer::Flush();
}

void SandboxLayer::OnMouseMoveEvent(uint32_t width, uint32_t height)
{
    this->x = width;
    this->y = height;
}
