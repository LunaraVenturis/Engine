#include "SandboxLayer.hpp"
#include "Engine.hpp"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

void SandboxLayer::Init(std::filesystem::path workingDirectory)
{
    using namespace LunaraEngine;

    std::filesystem::path assetsDirectory = workingDirectory / std::filesystem::path("Assets");

    m_Window = Renderer::GetWindow();
    AudioManager::LoadAudio("AudioTest", "Assets/Audio/AudioTest.wav");
    auto result = LoadFont("Assets/Fonts/joystixmonospace.ttf", 24, &m_Font);
    if (result != FontResultType::FONT_RESULT_SUCCESS) { exit(-6); }

    BatchRenderer::Create(assetsDirectory / "Shaders/output");
}

void SandboxLayer::Destroy()
{
    using namespace LunaraEngine;
    BatchRenderer::Destroy();
}

void SandboxLayer::OnUpdate(float dt)
{
    using namespace LunaraEngine;
    Renderer::Clear(Color4{0.0f, 0.0f, 0.0f, 1.0f});

    Renderer::BeginRenderPass();
    elapsedTime += dt;
    m_PlayerDt = dt;

    m_Player.Draw();
    m_Enemy.Draw();
    m_Wall.Draw();
    if (m_Player.isColliding(&m_Enemy)) { m_Player.SetPlayerColor({1.0f, 0.0f, 0.0f, 1.0f}); }
    else { m_Player.SetPlayerColor({1.0f, 1.0f, 1.0f, 1.0f}); }
    auto shader = BatchRenderer::GetShader();
    if (!shader.expired())
    {
        auto shaderPtr = shader.lock().get();
        Renderer::BindShader(shaderPtr, (void*) nullptr);
        m_Camera.Upload(shaderPtr);

        Renderer::DrawQuadBatch();
    }

    Renderer::EndRenderPass();

    //Flush command buffer for drawing
    Renderer::Flush();
    //Play audio test
    if (!LunaraEngine::AudioManager::IsAudioPlaying("AudioTest"))
    {
        LunaraEngine::AudioManager::PlayAudio("AudioTest");
    }
    if (m_Enemy.HasReachedPointX(m_Enemy.GetPosition().x, 128.0f))
    {
        m_goingRight = true;
    }
    if (m_Enemy.HasReachedPointX(m_Enemy.GetPosition().x, 0.0f)) { m_goingRight = false; }
    if (m_goingRight) { m_Enemy.Move(-1.0f, 0.0f, 0.0f, dt); }
    else { m_Enemy.Move(1.0f, 0.0f, 0.0f, dt); }

    if (m_PressedKeys[KEY_W]) { m_Player.Move(0.0f, -1.0f, 0.0f, dt); }
    if (m_PressedKeys[KEY_S]) { m_Player.Move(0.0f, 1.0f, 0.0f, dt); }
    if (m_PressedKeys[KEY_A]) { m_Player.Move(-1.0f, 0.0f, 0.0f, dt); }
    if (m_PressedKeys[KEY_D]) { m_Player.Move(1.0f, 0.0f, 0.0f, dt); }
    if (m_PressedKeys[KEY_SPACE]) { m_Player.Move(0.0f, 0.0f, 1.0f, dt); }
    if (m_PressedKeys[KEY_L])
    {
        zoom += dt;
        m_Camera.SetZoom(zoom);
        LOG_INFO("Zoom: %f", zoom);
    }
    else if (m_PressedKeys[KEY_K])
    {
        zoom -= dt;
        m_Camera.SetZoom(zoom);
        LOG_INFO("Zoom: %f", zoom);
    }
    else if (m_PressedKeys[KEY_F])
    {
        LOG_INFO("FPS: %f", 1.0f / dt);
        LOG_INFO("Position of Player x: %f ", m_Player.GetPosition().x);
        LOG_INFO("Position of Player y: %f ", m_Player.GetPosition().y);
        LOG_INFO("Position of Player z: %f ", m_Player.GetPosition().z);
        LOG_INFO("Position of Enemy x: %f ", m_Enemy.GetPosition().x);
        LOG_INFO("Position of Enemy y: %f ", m_Enemy.GetPosition().y);
        LOG_INFO("Position of Enemy z: %f ", m_Enemy.GetPosition().z);
        LOG_INFO("Width of Player: %f ", m_Player.GetSize().width);
        LOG_INFO("Height of Player: %f ", m_Player.GetSize().height);
        LOG_INFO("Width of Enemy: %f ", m_Enemy.GetSize().width);
        LOG_INFO("Height of Enemy: %f ", m_Enemy.GetSize().height);
    }
}

void SandboxLayer::OnMouseMoveEvent(uint32_t width, uint32_t height)
{
    this->x = width;
    this->y = height;
}

void SandboxLayer::OnWindowResizeEvent(uint32_t width, uint32_t height) { m_Camera.OnResize(width, height); }

void SandboxLayer::OnKeyboardEvent(uint32_t key, KeyEventType type)
{
    if (type == KEY_PRESSED) { m_PressedKeys[key] = true; }
    else { m_PressedKeys[key] = false; }
}
