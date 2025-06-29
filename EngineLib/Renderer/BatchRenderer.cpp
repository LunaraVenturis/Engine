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

        ShaderResources basicShaderResources;
        // basicShaderResources.bufferResources.push_back(ShaderResource{
        //         .type = ShaderResourceType::UniformBuffer,
        //         .name = "PushConstants",
        //         .size = sizeof(glm::mat4) * 2,
        //         .attributes = {
        //                 ShaderResourceAttribute{.name = "view_model", .type = ShaderResourceAttributeType::Mat4},
        //                 ShaderResourceAttribute{.name = "projection", .type = ShaderResourceAttributeType::Mat4},
        //         }});
        basicShaderResources.bufferResources.push_back(ShaderResource{
                .type = ShaderResourceType::UniformBuffer,
                .name = "UniformBuffer",
                .length = 1,
                .stride = sizeof(glm::mat4) * 3 + sizeof(float),
                .layout = ShaderResourceLayout{.binding = 0, .layoutType = ShaderResourceMemoryLayout::STD430},
                .attributes = {
                        ShaderResourceAttribute{.name = "model", .type = ShaderResourceAttributeType::Mat4},
                        ShaderResourceAttribute{.name = "view", .type = ShaderResourceAttributeType::Mat4},
                        ShaderResourceAttribute{.name = "projection", .type = ShaderResourceAttributeType::Mat4},
                        ShaderResourceAttribute{.name = "zoom", .type = ShaderResourceAttributeType::Float},
                }});
        basicShaderResources.bufferResources.push_back(ShaderResource{
                .type = ShaderResourceType::StorageBuffer,
                .name = "StorageBuffer",
                .length = MAX_QUADS,
                .stride = sizeof(glm::vec4) + sizeof(glm::vec3),
                .layout = ShaderResourceLayout{.binding = 1, .layoutType = ShaderResourceMemoryLayout::STD430},
                .attributes = {
                        ShaderResourceAttribute{.name = "quad", .type = ShaderResourceAttributeType::Vec4},
                        ShaderResourceAttribute{.name = "color", .type = ShaderResourceAttributeType::Vec4},
                }});
        ShaderInfo basicShaderInfo;
        basicShaderInfo.isComputeShader = false;
        basicShaderInfo.name = L"FlatQuadBatched";
        basicShaderInfo.path = shaderPath;
        basicShaderInfo.resources = basicShaderResources;


        s_BatchRenderer->m_Shader = Shader::Create(basicShaderInfo);
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
                {(StorageBuffer<uint8_t>*) shader->GetBuffer(1),
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
