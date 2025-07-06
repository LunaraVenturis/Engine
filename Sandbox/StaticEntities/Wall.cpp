#include "Wall.hpp"
#include "Engine.hpp"

Wall::Wall(glm::vec3 Position, WallSize Size) : m_Position(Position), m_Size(Size) {}

void Wall::Draw()
{
    auto rect = LunaraEngine::FRect{m_Position.x, m_Position.y, m_Size.width, m_Size.height};
    auto color = LunaraEngine::Color4{1.0f, 1.0f, 1.0f, 1.0f};
    LunaraEngine::BatchRenderer::AddQuad(rect, color);
}

bool Wall::isColliding(Entity* entity)
{
    const float Z_THRESHOLD = 0.1f;
    return (m_Position.x < entity->GetPosition().x + entity->GetSize().width &&
            m_Position.x + m_Size.width > entity->GetPosition().x &&
            m_Position.y < entity->GetPosition().y + entity->GetSize().height &&
            m_Position.y + m_Size.height > entity->GetPosition().y) &&
           fabs(m_Position.z - entity->GetPosition().z) < Z_THRESHOLD;
}