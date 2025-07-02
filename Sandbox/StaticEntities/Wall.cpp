#include "Wall.hpp"
#include "Engine.hpp"

Wall::Wall(glm::vec3 Position, WallSize Size) : m_Position(Position), m_Size(Size) {}

void Wall::Draw()
{
    auto rect = LunaraEngine::FRect{m_Position.x, m_Position.y, m_Size.width, m_Size.height};
    auto color = LunaraEngine::Color4{1.0f, 1.0f, 1.0f, 1.0f};
    LunaraEngine::BatchRenderer::AddQuad(rect, color);
}
