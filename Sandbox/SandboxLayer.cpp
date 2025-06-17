#include "SandboxLayer.hpp"
#include "Engine.hpp"
#include <glm/glm.hpp>

void SandboxLayer::Init(std::filesystem::path workingDirectory)
{
    using namespace LunaraEngine;

    std::filesystem::path assetsDirectory = workingDirectory / std::filesystem::path("Assets");

    m_Window = Renderer::GetWindow();
    AudioManager::LoadAudio("AudioTest", "Assets/Audio/AudioTest.wav");
    auto result = LoadFont("Assets/Fonts/joystixmonospace.ttf", 24, &m_Font);
    if (result != FontResultType::FONT_RESULT_SUCCESS) { exit(-6); }
    m_Player.Init();
    m_Shader = Shader::Create(ShaderType::FlatQuad, assetsDirectory);
}

void SandboxLayer::OnUpdate(float dt)
{
    using namespace LunaraEngine;

    Renderer::BeginRenderPass();

    Renderer::Clear(Color4{0.0f, 0.0f, 0.0f, 1.0f});
    elapsedTime += dt;
    m_PlayerDt = dt;
    
    glm::vec2 offset = glm::vec2{m_Player.GetPlayerPosition()};
    m_Shader->SetUniform("offset", offset);
    m_Camera.Upload(m_Shader.get());
    Renderer::BindShader(m_Shader.get());
    m_Player.Draw();


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

void SandboxLayer::OnKeyboardEvent(uint32_t key, KeyEventType type)
{
    LOG_INFO("Key: %d, Type: %d", key, type);
    if (type == KeyEventType::KEY_PRESSED)
    {
        if (key == KEY_D) { m_Player.MovePlayer(1.0f, 0, m_PlayerDt); }
        else if (key == KEY_A) { m_Player.MovePlayer(-1.0f, 0, m_PlayerDt); }
        if (key == KEY_W) { m_Player.MovePlayer(0, -1.0f, m_PlayerDt); }
        else if (key == KEY_S) { m_Player.MovePlayer(0, 1.0f, m_PlayerDt); }
    }
}

void SandboxLayer::OnWindowResizeEvent(uint32_t width, uint32_t height) { m_Camera.OnResize(width, height); }
