#include "BatchRenderer.hpp"

namespace LunaraEngine
{
    QuadBatchRenderer::QuadBatchRenderer()
    {
        m_QuadVerts.resize(MAX_QUADS);
        m_QuadColors.resize(MAX_QUADS);
        m_Capacity = MAX_QUADS;
        m_Offset = 0;
        m_QuadCount = 0;
    }

    QuadBatchRenderer* QuadBatchRenderer::GetInstance() { return s_QuadBatchRenderer; }

    void QuadBatchRenderer::DrawQuad(const FRect& rect, const Color4& color)
    {
        auto instance = GetInstance();
        if (instance->m_QuadCount == instance->m_Capacity) { return; }

        instance->m_QuadVerts[instance->m_QuadCount] = rect;
        instance->m_QuadColors[instance->m_QuadCount] = color;
        ++instance->m_QuadCount;
    }

}// namespace LunaraEngine
