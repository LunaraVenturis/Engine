#include "SandboxLayer.hpp"
#include "Engine.hpp"
#include <glm/glm.hpp>

void SandboxLayer::Init(std::filesystem::path workingDirectory)
{
    using namespace LunaraEngine;

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

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    ShaderResources basicShaderResources;
    basicShaderResources.bufferResources.push_back(ShaderResource{
            .type = ShaderResourceType::UniformBuffer,
            .name = "UniformBuffer",
            .size = sizeof(UniformBufferObject),
            .layout = ShaderResourceLayout{.binding = 0, .layoutType = ShaderResourceMemoryLayout::STD140},
            .attributes = {
                    ShaderResourceAttribute{.name = "offset", .type = ShaderResourceAttributeType::Vec3},
            }});

    ShaderInfo basicShaderInfo;
    basicShaderInfo.isComputeShader = false;
    basicShaderInfo.name = L"BasicShader";
    basicShaderInfo.path = workingDirectory / "Assets/Shaders/output";
    basicShaderInfo.resources = basicShaderResources;

    basicShaderInfo.resources.inputResources.push_back(ShaderInputResource{.name = "Position",
                                                                           .binding = 0,
                                                                           .location = 0,
                                                                           .format = ShaderResourceFormatT::R32G32,
                                                                           .type = ShaderResourceDataTypeT::SFloat,
                                                                           .offset = 0});
    basicShaderInfo.resources.inputResources.push_back(ShaderInputResource{.name = "Color",
                                                                           .binding = 0,
                                                                           .location = 1,
                                                                           .format = ShaderResourceFormatT::R32G32B32,
                                                                           .type = ShaderResourceDataTypeT::SFloat,
                                                                           .offset = 8});
    m_Shader.Init(basicShaderInfo);
}

void SandboxLayer::OnUpdate(float dt)
{
    (void) dt;
    using namespace LunaraEngine;

    Renderer::BeginRenderPass();

    Renderer::Clear(Color4{0.0f, 0.0f, 0.0f, 1.0f});
    elapsedTime += dt;
    glm::vec3 offset = glm::vec3{sin(elapsedTime), 0.0f, 0.0f};
    m_Shader.SetUniform("offset", offset);

    Renderer::DrawIndexed(&m_Shader, &m_QuadBuffer, &m_QuadIndexBuffer);

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
