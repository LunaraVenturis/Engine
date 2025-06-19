#include "Enemy.hpp"
#include "Engine.hpp"
Enemy::Enemy(glm::vec2 enemyPosition, EntitySize enemySize)
    : m_Position(enemyPosition), m_Speed({2.0f, 2.0f}), m_size(enemySize)
{

}
void Enemy::Draw() 
{
    auto rect = LunaraEngine::FRect{m_Position.x, m_Position.y, m_size.width, m_size.height};
    auto color = LunaraEngine::Color4{1.0f, 1.0f, 1.0f, 1.0f};
    LunaraEngine::BatchRenderer::AddQuad(rect, color);
}
void Enemy::Move(f32 x, f32 y, f32 delta)
{
    m_Position.x += (x * m_Speed.x * delta);
    m_Position.y += (y * m_Speed.y * delta);
}
