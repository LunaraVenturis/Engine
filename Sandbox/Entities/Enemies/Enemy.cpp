#include "Enemy.hpp"
#include "Engine.hpp"
Enemy::Enemy(glm::vec3 enemyPosition, EntitySize enemySize)
    : m_Position(enemyPosition), m_Speed({100.0f, 100.0f, 100.0f}), m_size(enemySize)
{

}
void Enemy::Draw() 
{
    auto rect = LunaraEngine::FRect{m_Position.x, m_Position.y, m_size.width, m_size.height};
    auto color = LunaraEngine::Color4{1.0f, 1.0f, 1.0f, 1.0f};
    LunaraEngine::BatchRenderer::AddQuad(rect, color);
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
    if(std::isnan(currentX) || std::isnan(destX))
    {
        LOG_ERROR("Invalid point");
        return 0;
    }
    return fabs(currentX - destX) < fltTolerance;
}