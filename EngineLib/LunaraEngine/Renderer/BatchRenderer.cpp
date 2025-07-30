#include "BatchRenderer.hpp"
#include <LunaraEngine/Renderer/Buffer/IndexBuffer.hpp>
#include <LunaraEngine/Renderer/Shader.hpp>
#include <LunaraEngine/Renderer/Buffer/StorageBuffer.hpp>
#include <LunaraEngine/Renderer/RendererCommands.hpp>
#include <glm/glm.hpp>
#include <span>

namespace LunaraEngine
{
    BatchRenderer::BatchRenderer()
    {
        m_Capacity = MAX_QUADS;
        m_Offset = 0;
        m_QuadCount = 0;
        m_Positions.resize(m_Capacity);
        m_Sizes.resize(m_Capacity);
        m_TextureIndices.resize(m_Capacity);
        std::fill(m_Positions.begin(), m_Positions.end(), glm::vec4(0.0f));
        std::fill(m_Sizes.begin(), m_Sizes.end(), glm::vec2(0.0f));
        std::fill(m_TextureIndices.begin(), m_TextureIndices.end(), 0);
    }

    BatchRenderer::BatchRenderer(const ApplicationConfig& config, std::vector<std::wstring_view> textureNames)
        : BatchRenderer()
    {
        Create(config, textureNames);
    }

    void BatchRenderer::Create(const ApplicationConfig& config, std::vector<std::wstring_view> textureNames)
    {
        m_Shader = Shader::Create(
                ShaderInfoBuilder("FlatQuadBatched", config.shadersDirectory)
                        .AddResources(
                                {BufferResourceBuilder("UniformBuffer", BufferResourceType::UniformBuffer)
                                         .AddAttributes({{"model", BufferResourceAttributeType::Mat4},
                                                         {"view", BufferResourceAttributeType::Mat4},
                                                         {"projection", BufferResourceAttributeType::Mat4},
                                                         {"zoom", BufferResourceAttributeType::Float}})
                                         .Build(),
                                 BufferResourceBuilder("Positions", BufferResourceType::StorageBuffer, MAX_QUADS)
                                         .AddAttributes({{"Position", BufferResourceAttributeType::Vec4}})
                                         .Build(),
                                 BufferResourceBuilder("Sizes", BufferResourceType::StorageBuffer, MAX_QUADS)
                                         .AddAttributes({{"Size", BufferResourceAttributeType::Vec2}})
                                         .Build(),
                                 BufferResourceBuilder("TextureIndices", BufferResourceType::StorageBuffer, MAX_QUADS)
                                         .AddAttributes({{"TextureIndex", BufferResourceAttributeType::UInt}})
                                         .Build()})
                        .AddResource(TextureResourceBuilder<TextureResourceType::Texture2DArray>(
                                             "Texture2DArray", config.texturesDirectory, textureNames)
                                             .Build())
                        .Build());
    }

    void BatchRenderer::Destroy() {}

    void BatchRenderer::AddQuad(const glm::vec3&& position, const glm::vec2&& size, const uint32_t textureIndex)
    {
        if (m_QuadCount == m_Capacity) { return; }

        m_Positions[m_QuadCount] = glm::vec4{position, 0.0f};
        m_Sizes[m_QuadCount] = size;
        m_TextureIndices[m_QuadCount] = textureIndex;
        ++m_QuadCount;
    }

    RendererCommandDrawBatch* BatchRenderer::CreateDrawCommand()
    {
        BufferUploadListBuilder uploadListBuilder(m_Shader);
        uploadListBuilder.Add(m_Positions, m_Sizes, m_TextureIndices);

        RendererCommandDrawBatch* command =
                new RendererCommandDrawBatch(uploadListBuilder.Get(), m_QuadCount, m_Offset);

        return command;
    }

    void BatchRenderer::Flush()
    {
        m_Offset = 0;
        m_QuadCount = 0;
    }

    std::weak_ptr<Shader> BatchRenderer::GetShader() { return m_Shader; }

}// namespace LunaraEngine
