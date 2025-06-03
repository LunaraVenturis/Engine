#pragma once
#include <cstdint>
#include <vector>

#include <Math/Color.h>
#include <Math/Rect.h>

namespace LunaraEngine
{
    using std::size_t;

    class QuadBatchRenderer
    {
    public:
        QuadBatchRenderer();
        ~QuadBatchRenderer() = default;

    public:
        static QuadBatchRenderer* GetInstance();

    public:
        static void DrawQuad(const FRect& rect, const Color4& color);

    private:
        static QuadBatchRenderer* s_QuadBatchRenderer;

        static constexpr size_t MAX_QUADS = 1000;
        size_t m_Offset{};
        size_t m_QuadCount{};
        size_t m_Capacity;

        std::vector<FRect> m_QuadVerts;
        std::vector<Color4> m_QuadColors;
    };
}// namespace LunaraEngine