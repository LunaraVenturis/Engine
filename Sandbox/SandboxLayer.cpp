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
    m_Player.Init();
    m_Shader = Shader::Create(ShaderType::FlatQuad, assetsDirectory);

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
    
    glm::vec2 offset = glm::vec2{m_Player.GetPlayerPosition()};
    m_Shader->SetUniform("offset", offset);
    m_Camera.Upload(m_Shader.get());
    Renderer::BindShader(m_Shader.get());
    m_Player.Draw();


    glm::vec2 camPos = {0.f, 0.f};
    glm::mat4 view = glm::translate(glm::mat4(1.0f), -glm::vec3(camPos.x, camPos.y, 0.0f));
    glm::mat4 projection = glm::ortho(0.0f, 1280.0f * zoom, 0.0f, 720.0f * zoom);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));


    constexpr size_t batchWidth = 100;
    constexpr size_t batchHeight = 100;
    constexpr float offset = 1.0f;
    constexpr size_t size = 100;
    constexpr Color4 color = {0.2f, 0.3f, 0.6f, 1.0};
    for (size_t i = 0; i < batchWidth; i++)
    {
        for (size_t j = 0; j < batchHeight; j++)
        {
            FRect rect = FRect{(float) (size / 2 + offset) * (float) i, (float) (size / 2 + offset) * (float) j,
                               (float) size, (float) size};
            BatchRenderer::AddQuad(rect, color);
        }
    }

    // struct PushConstants {
    // glm::mat4 view_model;
    // glm::mat4 projection;
    // } constants;

    auto shader = BatchRenderer::GetShader();
    if (!shader.expired())
    {
        auto shaderPtr = shader.lock().get();


        Renderer::BindShader(shaderPtr, (void*) nullptr);
        shaderPtr->SetUniform("projection", projection);
        shaderPtr->SetUniform("view", view);
        shaderPtr->SetUniform("model", model);

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
    if (m_PressedKeys[KEY_L])
    {
        zoom += dt;
        LOG_INFO("Zoom: %f", zoom);
    }
    else if (m_PressedKeys[KEY_K])
    {
        zoom -= dt;
        LOG_INFO("Zoom: %f", zoom);
    }
    else if (m_PressedKeys[KEY_F]) { LOG_INFO("FPS: %f", 1.0f / dt); }
}

void SandboxLayer::OnMouseMoveEvent(uint32_t width, uint32_t height)
{
    this->x = width;
    this->y = height;
}

void SandboxLayer::OnKeyboardEvent(uint32_t key, KeyEventType type)
{
    if (type == KEY_PRESSED) { m_PressedKeys[key] = true; }
    else { m_PressedKeys[key] = false; }
}
