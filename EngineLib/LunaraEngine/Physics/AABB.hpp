#pragma once
#include <LunaraEngine/Core/CommonTypes.hpp>
#include "glm/glm.hpp"

namespace LunaraEngine
{
    class AABB
    {
    public:
        AABB() = default;
        AABB(const AABB& other) = delete;
        AABB(AABB&& other) = delete;
        AABB& operator=(const AABB& other) = delete;
        AABB& operator=(AABB&& other) = delete;

    public:
        void Init(const glm::vec3& ObjOnePos, f32 ObjOneW, f32 ObjOneH, const glm::vec3& ObjTwoPos, f32 ObjTwoW,
                  f32 ObjTwoH);
        bool Intersection();

    private:
        glm::vec3 m_ObjOnePosition;
        glm::vec3 m_ObjTwoPosition;
        f32 m_ObjOneWidth;
        f32 m_ObjOneHeight;
        f32 m_ObjTwoWidth;
        f32 m_ObjTwoHeight;
    };
}// namespace LunaraEngine