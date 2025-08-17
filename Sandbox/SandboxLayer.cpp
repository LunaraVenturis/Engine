#include "SandboxLayer.hpp"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

void SandboxLayer::Init(const LunaraEngine::ApplicationConfig& config)
{
    using namespace LunaraEngine;

    m_Window = Renderer::GetWindow();
    AudioManager::LoadAudio("AudioTest", "Assets/Audio/AudioTest.wav");
    auto result = LoadFont("Assets/Fonts/joystixmonospace.ttf", 24, &m_Font);
    if (result != FontResultType::FONT_RESULT_SUCCESS) { exit(-6); }

    std::vector<std::wstring_view> coin_sprites = {
            L"coin_sprites/coin_r0_c0.png", L"coin_sprites/coin_r0_c1.png",  L"coin_sprites/coin_r0_c2.png",
            L"coin_sprites/coin_r0_c3.png", L"coin_sprites/coin_r0_c4.png",  L"coin_sprites/coin_r0_c5.png",
            L"coin_sprites/coin_r0_c6.png", L"coin_sprites/coin_r0_c7.png",  L"coin_sprites/coin_r0_c8.png",
            L"coin_sprites/coin_r0_c9.png", L"coin_sprites/coin_r0_c10.png", L"coin_sprites/coin_r0_c11.png"};
    std::vector<std::wstring_view> sonic_walking_sprites = {
            L"sonic_sprites/sonic_r0_c0.png", L"sonic_sprites/sonic_r0_c1.png", L"sonic_sprites/sonic_r0_c2.png",
            L"sonic_sprites/sonic_r0_c3.png", L"sonic_sprites/sonic_r0_c4.png", L"sonic_sprites/sonic_r0_c5.png"};

    std::wstring_view wall_sprite = L"world_tileset_sprites/world_tileset_r14_c3.png";

    std::vector<std::wstring_view> enemy_sprites = {
            L"slime_purple_sprites/slime_purple_r1_c0.png", L"slime_purple_sprites/slime_purple_r1_c1.png",
            L"slime_purple_sprites/slime_purple_r1_c2.png", L"slime_purple_sprites/slime_purple_r1_c3.png"};

    std::vector<std::wstring_view> batch_renderer_sprites;
    batch_renderer_sprites.insert(batch_renderer_sprites.end(), coin_sprites.begin(), coin_sprites.end());
    batch_renderer_sprites.insert(
            batch_renderer_sprites.end(), sonic_walking_sprites.begin(), sonic_walking_sprites.end());
    batch_renderer_sprites.insert(batch_renderer_sprites.end(), enemy_sprites.begin(), enemy_sprites.end());
    batch_renderer_sprites.push_back(wall_sprite);

    m_BatchRenderer = std::make_shared<BatchRenderer>();
    m_BatchRenderer->Create(config, batch_renderer_sprites);

    m_Player.SetAnimationFrameCount((u32) sonic_walking_sprites.size());
    m_Player.SetSpriteSheetIndex((u32) coin_sprites.size());

    m_Enemy.SetAnimationFrameCount((u32) enemy_sprites.size());
    m_Enemy.SetSpriteSheetIndex((u32) sonic_walking_sprites.size() + (u32) coin_sprites.size());

    m_Wall.SetSpriteSheetIndex((u32) batch_renderer_sprites.size() - 1);

    m_Coin.SetSpriteSheetIndex(0);
    m_Coin.SetAnimationFrameCount((u32) coin_sprites.size());
    // ParticleSystem::Create(config);
}

void SandboxLayer::Destroy()
{
    using namespace LunaraEngine;
    ParticleSystem::Destroy();
     //m_BatchRenderer->Destroy();
}

void SandboxLayer::OnUpdate(float dt)
{
    using namespace LunaraEngine;

    // ParticleSystem::GetInstance()->Update(dt);

    Renderer::Clear(Color4{0.0f, 0.0f, 0.0f, 1.0f});

    Renderer::BeginRenderPass();
    elapsedTime += dt;
    m_PlayerDt = dt;

    m_Coin.Draw(m_BatchRenderer);
    m_Player.Draw(m_BatchRenderer);
    m_Enemy.Draw(m_BatchRenderer);
    m_Wall.Draw(m_BatchRenderer);
    //m_PlayerCollider.Intersection(m_Player.GetPosition(), m_Player.GetEntity().width, m_Player.GetEntity().height, m_Enemy.GetPosition(), m_Enemy.GetEntity().width, m_Enemy.GetEntity().height);
    // if (m_PlayerCollider.Intersection()) { m_Player.SetPlayerColor({1.0f, 0.0f, 0.0f, 1.0f}); }
    // else { m_Player.SetPlayerColor({1.0f, 1.0f, 1.0f, 1.0f}); }
    if (m_Wall.isColliding(&m_Player))
    {
        m_Player.SetPosition({m_Player.GetPosition().x - 20, m_Player.GetPosition().y - 20, 0.0f});
    }
    if (m_Enemy.HasReachedPointX(m_Enemy.GetPosition().x, 200.0f)) { dir = -1.0f; }
    if (m_Enemy.HasReachedPointX(m_Enemy.GetPosition().x, 0.0f)) { dir = 1.0f; }
    m_Enemy.Move(dir * 1.0f, 0.0f, 0.0f, dt);

    auto shader = m_BatchRenderer->GetShader();
    if (!shader.expired())
    {
        auto shaderPtr = shader.lock().get();
        Renderer::BindShader(shaderPtr, (void*) nullptr);
        m_Camera.Upload(shaderPtr);

        Renderer::DrawQuadBatch(m_BatchRenderer);
    }

    // auto particleShader = ParticleSystem::GetShader();
    // if (!particleShader.expired())
    // {
    //     auto shaderPtr = particleShader.lock().get();
    //     Renderer::BindShader(shaderPtr, (void*) nullptr);
    //     m_Camera.Upload(shaderPtr);

    //     Renderer::PushCommand(ParticleSystem::CreateDrawCommand());
    // }

    Renderer::EndRenderPass();

    //Flush command buffer for drawing
    Renderer::Flush();

    m_Player.UpdateAnimation(dt);
    m_Coin.UpdateAnimation(dt);
    m_Enemy.UpdateAnimation(dt);

    //Play audio test
    if (!LunaraEngine::AudioManager::IsAudioPlaying("AudioTest"))
    {
        LunaraEngine::AudioManager::PlayAudio("AudioTest");
    }

    if (m_PressedKeys[KEY_W]) { m_Player.Move(0.0f, -1.0f, 0.0f, dt); }
    if (m_PressedKeys[KEY_S]) { m_Player.Move(0.0f, 1.0f, 0.0f, dt); }
    if (m_PressedKeys[KEY_A]) { m_Player.Move(-1.0f, 0.0f, 0.0f, dt); }
    if (m_PressedKeys[KEY_D]) { m_Player.Move(1.0f, 0.0f, 0.0f, dt); }

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
    else if (m_PressedKeys[KEY_F]) { LOG_INFO("FPS: %f", 1.0f / dt); }
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