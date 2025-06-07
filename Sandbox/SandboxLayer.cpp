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

    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;
    };

    std::vector<Vertex> vertices = {{glm::vec2{-0.5f, -0.5f}, glm::vec3{1.0f, 0.0f, 0.0f}},
                                    {glm::vec2{0.5f, -0.5f}, glm::vec3{0.0f, 1.0f, 0.0f}},
                                    {glm::vec2{0.5f, 0.5f}, glm::vec3{0.0f, 0.0f, 1.0f}},
                                    {glm::vec2{-0.5f, 0.5f}, glm::vec3{1.0f, 1.0f, 1.0f}}};

    m_QuadBuffer.Create({{"Position", VertexAttributeType::VEC3}, {"Color", VertexAttributeType::VEC3}},
                        (uint8_t*) vertices.data(), vertices.size());
    std::vector<uint16_t> indices{0, 1, 2, 2, 3, 0};
    m_QuadIndexBuffer.Create(indices.data(), indices.size());

    m_Shader = Shader::Create(ShaderType::FlatInstanced, assetsDirectory);
}

void SandboxLayer::OnUpdate(float dt)
{
    using namespace LunaraEngine;

    Renderer::BeginRenderPass();

    Renderer::Clear(Color4{0.0f, 0.0f, 0.0f, 1.0f});
    elapsedTime += dt;
    glm::vec2 offset = glm::vec2{(sin(elapsedTime) + cos(elapsedTime)) / 2, (cos(elapsedTime) - sin(elapsedTime)) / 2};
    m_Shader->SetUniform("offset", offset);

    Renderer::BindShader(m_Shader.get());
    // Renderer::DrawIndexed(&m_QuadBuffer, &m_QuadIndexBuffer);

    Renderer::DrawInstanced(&m_QuadBuffer, &m_QuadIndexBuffer, 5);
    // Renderer::DrawQuad(FRect{300.0f, 300.0f, 100.0f, 100.0f}, Color4{1.0f, 0.0f, 0.0f, 1.0f});

    // size_t length = (size_t) snprintf(NULL, 0, "%u, %u", x, y);
    // text[length] = '\0';
    // snprintf(text, length + 1, "%d, %d", x, y);

    // std::string_view t = std::string_view(text, length);
    // Renderer::DrawText(t, &m_Font, 100.0f, 100.0f, Color4{1.0f, 1.0f, 1.0f, 1.0f});

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

void SandboxLayer::OnKeyboardEvent(uint32_t key, KeyEventType type) { LOG_INFO("Key: %d, Type: %d", key, type); }
