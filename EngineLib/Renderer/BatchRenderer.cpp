#include "BatchRenderer.hpp"
#include <Renderer/Buffer/IndexBuffer.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Buffer/StorageBuffer.hpp>
#include <Renderer/RendererCommands.hpp>
#include <glm/glm.hpp>
#include <span>

namespace LunaraEngine
{
    BatchRenderer::BatchRenderer()
    {
        m_Capacity = MAX_QUADS;
        m_Offset = 0;
        m_QuadCount = 0;
        m_Quads.resize(m_Capacity);
    }

    void BatchRenderer::Create(std::filesystem::path shaderPath, std::filesystem::path texturesPath)
    {
        s_BatchRenderer = new BatchRenderer();

        s_BatchRenderer->m_Shader = Shader::Create(
                ShaderInfoBuilder("FlatQuadBatched", shaderPath)
                        .AddResources(
                                {BufferResourceBuilder("UniformBuffer", BufferResourceType::UniformBuffer)
                                         .AddAttributes({{"model", BufferResourceAttributeType::Mat4},
                                                         {"view", BufferResourceAttributeType::Mat4},
                                                         {"projection", BufferResourceAttributeType::Mat4},
                                                         {"zoom", BufferResourceAttributeType::Float}})
                                         .Build(),
                                 BufferResourceBuilder("StorageBuffer", BufferResourceType::StorageBuffer, MAX_QUADS)
                                         .AddAttributes({{"quad", BufferResourceAttributeType::Vec4},
                                                         {"color", BufferResourceAttributeType::Vec4}})
                                         .Build()})
                        .AddResource(TextureResourceBuilder("Texture", BufferResourceType::Texture2D,
                                                            TextureInfo{.path = texturesPath, .name = L"block.png"})
                                             .Build())
                        .Build());
    }

    void BatchRenderer::Destroy() { delete s_BatchRenderer; }

    BatchRenderer* BatchRenderer::GetInstance() { return s_BatchRenderer; }

    void BatchRenderer::AddQuad(const FRect& rect, const Color4& color)
    {
        auto instance = GetInstance();
        if (instance->m_QuadCount == instance->m_Capacity) { return; }

        instance->m_Quads[instance->m_QuadCount] = {rect, color};
        ++instance->m_QuadCount;
    }

    RendererCommandDrawBatch* BatchRenderer::CreateDrawCommand()
    {
        auto instance = GetInstance();

        BufferUploadListBuilder uploadListBuilder(std::weak_ptr<Shader>(instance->m_Shader));
        uploadListBuilder.Add(instance->m_Quads.data(), instance->m_QuadCount);

        RendererCommandDrawBatch* command =
                new RendererCommandDrawBatch(uploadListBuilder.Get(), instance->m_QuadCount, instance->m_Offset);

        return command;
    }

    void BatchRenderer::Flush()
    {
        auto instance = BatchRenderer::GetInstance();
        instance->m_Offset = 0;
        instance->m_QuadCount = 0;
    }

    std::weak_ptr<Shader> BatchRenderer::GetShader() { return s_BatchRenderer->m_Shader; }

}// namespace LunaraEngine
