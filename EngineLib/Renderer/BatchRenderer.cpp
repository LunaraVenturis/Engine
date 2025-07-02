#include "BatchRenderer.hpp"
#include <Renderer/IndexBuffer.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/StorageBuffer.hpp>
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

    void BatchRenderer::Create(std::filesystem::path shaderPath)
    {
        s_BatchRenderer = new BatchRenderer();

        s_BatchRenderer->m_Shader = Shader::Create(
                ShaderInfoBuilder("FlatQuadBatched", shaderPath)
                        .AddResources(
                                {ShaderResourceBuilder("UniformBuffer", ShaderResourceType::UniformBuffer)
                                         .AddAttributes({{"model", ShaderResourceAttributeType::Mat4},
                                                         {"view", ShaderResourceAttributeType::Mat4},
                                                         {"projection", ShaderResourceAttributeType::Mat4},
                                                         {"zoom", ShaderResourceAttributeType::Float}})
                                         .Build(),
                                 ShaderResourceBuilder("StorageBuffer", ShaderResourceType::StorageBuffer, MAX_QUADS)
                                         .AddAttributes({{"quad", ShaderResourceAttributeType::Vec4},
                                                         {"color", ShaderResourceAttributeType::Vec4}})
                                         .Build()})
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
        auto shader = instance->m_Shader;

        RendererCommandDrawBatch::BufferUploadList uploadList{
                {(StorageBuffer<uint8_t>*) shader->GetBuffer(ShaderBinding::_1),
                 std::span<uint8_t>{(uint8_t*) (instance->m_Quads.data()),
                                    instance->m_QuadCount * sizeof(QuadBufferElement)}}};

        RendererCommandDrawBatch* command =
                new RendererCommandDrawBatch(uploadList, instance->m_QuadCount, instance->m_Offset);

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
