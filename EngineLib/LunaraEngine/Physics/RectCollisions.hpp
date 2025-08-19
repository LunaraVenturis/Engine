#pragma once
#include <LunaraEngine/Core/CommonTypes.hpp>
#include "glm/glm.hpp"

namespace LunaraEngine
{
    class RectCollisions
    {
    public:
        RectCollisions() = default;

    public:
        bool  AABB(const glm::vec3& Player, f32 PlayerWidth, f32 PlayerHeight, const glm::vec3& Enemy, f32 EnemyWidth,
                          f32 EnemyHeight);
    };
}// namespace LunaraEngine
