#include "Enemy.hpp"
#include <LunaraEngine/Engine.hpp>

Enemy::Enemy(const glm::vec3& enemyPosition, const EntitySize enemySize)
    : m_Position(enemyPosition), m_Speed({100.0f, 100.0f, 100.0f}), m_Size(enemySize)
{}

void Enemy::Draw(std::weak_ptr<LunaraEngine::BatchRenderer> renderer)
{
    if (renderer.expired()) { return; }
    renderer.lock()->AddQuad(
            glm::vec3{m_Position}, glm::vec2{m_Size.width, m_Size.height}, p_StartTextureIndex + p_AnimationIndex);
}

void Enemy::UpdateAnimation(f32 delta)
{
    p_AnimationIndex = (uint32_t) fmod(p_AnimationTimer, p_MaxAnimationIndex);
    p_AnimationTimer += delta * (p_AnimationSpeed / p_AnimationFactor);
}

void Enemy::Move(f32 x, f32 y, f32 z, f32 delta)
{
    m_Position.x += (x * m_Speed.x * delta);
    m_Position.y += (y * m_Speed.y * delta);
    m_Position.z += (z * m_Speed.z * delta);
}

bool Enemy::HasReachedPointX(f32 currentX, f32 destX)
{
    const f32 fltTolerance = 1.0f;
    if (std::isnan(currentX) || std::isnan(destX))
    {
        LOG_ERROR("Invalid point");
        return 0;
    }
    return fabs(currentX - destX) < fltTolerance;
}