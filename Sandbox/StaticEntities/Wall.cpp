#include "Wall.hpp"
#include <LunaraEngine/Engine.hpp>

Wall::Wall(const glm::vec3& Position, const WallSize Size) : m_Position(Position), m_Size(Size) {}

void Wall::Draw(std::weak_ptr<LunaraEngine::BatchRenderer> renderer)
{
    if (renderer.expired()) { return; }
    renderer.lock()->AddQuad(glm::vec3{m_Position}, glm::vec2{m_Size.width, m_Size.height}, m_StartTextureIndex);
}

bool Wall::isColliding(Entity* entity)
{
    const float Z_THRESHOLD = 0.1f;
    return (m_Position.x < entity->GetPosition().x + entity->GetEntity().width &&
            m_Position.x + m_Size.width > entity->GetPosition().x &&
            m_Position.y < entity->GetPosition().y + entity->GetEntity().height &&
            m_Position.y + m_Size.height > entity->GetPosition().y) &&
           fabs(m_Position.z - entity->GetPosition().z) < Z_THRESHOLD;
}