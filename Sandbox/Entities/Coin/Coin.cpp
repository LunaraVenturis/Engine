#include "Coin.hpp"
#include <cmath>

Coin::Coin(const glm::vec3& coinPosition, const EntitySize coinSize, const FacingDirection direction)
    : m_Position(coinPosition), m_Speed({1000.0f, 1000.0f, 1000.0f}), m_Size(coinSize),
      m_Dir(static_cast<u32>(direction))
{}

void Coin::Draw(std::weak_ptr<LunaraEngine::BatchRenderer> renderer)
{
    if (renderer.expired()) { return; }
    renderer.lock()->AddQuad(
            glm::vec3{m_Position}, glm::vec2{m_Size.width, m_Size.height}, p_StartTextureIndex + p_AnimationIndex);
}

void Coin::UpdateAnimation(f32 delta)
{
    p_StartTextureIndex = (uint32_t) fmod(p_AnimationTimer, p_MaxAnimationIndex);
    p_AnimationTimer += delta * (p_AnimationSpeed / p_AnimationFactor);
}

void Coin::Move(f32 x, f32 y, f32 z, f32 delta)
{
    m_Position.x += (x * m_Speed.x * delta);
    m_Position.y += (y * m_Speed.y * delta);
    m_Position.z += (z * m_Speed.z * delta);
}

bool Coin::isColliding(Entity* enemy)
{
    const float Z_THRESHOLD = 0.1f;
    return (m_Position.x < enemy->GetPosition().x + enemy->GetEntity().width &&
            m_Position.x + m_Size.width > enemy->GetPosition().x &&
            m_Position.y < enemy->GetPosition().y + enemy->GetEntity().height &&
            m_Position.y + m_Size.height > enemy->GetPosition().y) &&
           fabs(m_Position.z - enemy->GetPosition().z) < Z_THRESHOLD;
}

void Coin::SetPosition(const glm::vec3& position) { m_Position = position; }