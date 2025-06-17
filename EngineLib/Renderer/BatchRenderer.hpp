#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <filesystem>

#include <Math/Color.h>
#include <Math/Rect.h>

namespace LunaraEngine
{

    struct QuadBufferElement {
        FRect Rect;
        Color4 Color;
    };

    using std::size_t;
    template <typename T>
    class IndexBuffer;
    template <typename T>
    class StorageBuffer;
    class RendererCommandDrawBatch;
    class Shader;

    class BatchRenderer
    {
    public:
        BatchRenderer();
        ~BatchRenderer() = default;

    public:
        static BatchRenderer* GetInstance();
        static void Create(std::filesystem::path shaderPath);
        static void Destroy();

    public:
        static void AddQuad(const FRect& rect, const Color4& color);
        static RendererCommandDrawBatch* GetDrawCommand();
        static void Flush();
        static std::weak_ptr<Shader> GetShader();

    private:
        inline static BatchRenderer* s_BatchRenderer = nullptr;

        static constexpr size_t MAX_QUADS = 100000;

    private:
        size_t m_Offset{};
        size_t m_QuadCount{};
        size_t m_Capacity{};

        std::vector<QuadBufferElement> m_Quads;

        std::shared_ptr<Shader> m_Shader;
    };
}// namespace LunaraEngine