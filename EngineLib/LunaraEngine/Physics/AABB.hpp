#pragma once
#include <LunaraEngine/Core/CommonTypes.hpp>
#include "glm/glm.hpp"

namespace LunaraEngine
{
    class AABB
    {
    public:
        AABB() = default;

    public:
        bool Intersection(const glm::vec3& Player, f32 PlayerWidth, f32 PlayerHeight, const glm::vec3& Enemy, f32 EnemyWidth,
                          f32 EnemyHeight);
    };
}// namespace LunaraEngine
