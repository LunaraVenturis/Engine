#include "RectCollisions.hpp"

namespace LunaraEngine
{


    bool RectCollisions::AABB(const glm::vec3& Player, f32 PlayerWidth, f32 PlayerHeight, const glm::vec3& Enemy, f32 EnemyWidth,
                    f32 EnemyHeight)
    {
        const float Z_THRESHOLD = 0.1f;
        return (Player.x < Enemy.x + EnemyWidth &&
                Player.x + PlayerWidth > Enemy.x &&
                Player.y < Enemy.y + EnemyHeight &&
                Player.y + PlayerHeight > Enemy.y) &&
               fabs(Player.z - Enemy.z) < Z_THRESHOLD;
    }
}// namespace LunaraEngine