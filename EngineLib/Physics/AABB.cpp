#include "AABB.hpp"

namespace LunaraEngine
{
    void AABB::Init(const glm::vec3& ObjOnePos, f32 ObjOneW, f32 ObjOneH, const glm::vec3& ObjTwoPos, f32 ObjTwoW,
             f32 ObjTwoH)
    {
        m_ObjOneHeight = ObjOneH;
        m_ObjOnePosition = ObjOnePos;
        m_ObjOneWidth = ObjOneW;
        m_ObjTwoHeight = ObjTwoH;
        m_ObjTwoPosition = ObjTwoPos;
        m_ObjTwoWidth = ObjTwoW;
    }
    bool AABB::Intersection()
    {
        const float Z_THRESHOLD = 0.1f;
        return (m_ObjOnePosition.x < m_ObjTwoPosition.x + m_ObjTwoWidth &&
                m_ObjOnePosition.x + m_ObjOneWidth > m_ObjTwoPosition.x &&
                m_ObjOnePosition.y < m_ObjTwoPosition.y + m_ObjTwoHeight &&
                m_ObjOnePosition.y + m_ObjOneHeight > m_ObjTwoPosition.y) &&
               fabs(m_ObjOnePosition.z - m_ObjTwoPosition.z) < Z_THRESHOLD;
    }
}