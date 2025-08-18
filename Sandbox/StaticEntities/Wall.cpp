#include "Wall.hpp"
#include <LunaraEngine/Engine.hpp>

Wall::Wall(const glm::vec3& Position, const WallSize Size) : m_Position(Position), m_Size(Size) {}

void Wall::Draw(std::weak_ptr<LunaraEngine::BatchRenderer> renderer)
{
    if (renderer.expired()) { return; }
    renderer.lock()->AddQuad(glm::vec3{m_Position}, glm::vec2{m_Size.width, m_Size.height}, m_StartTextureIndex);
}
