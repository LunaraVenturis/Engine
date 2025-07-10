#include "Player.hpp"
#include <Engine.hpp>

Player::Player(glm::vec3 playerPosition, EntitySize playerSize)
    : m_Position(playerPosition), m_Speed({1000.0f, 1000.0f, 1000.0f}), m_Size(playerSize),
      m_PlayerColor({1.0f, 1.0f, 1.0f, 1.0f})
{}

void Player::Draw()
{
    auto rect = LunaraEngine::FRect{m_Position.x, m_Position.y, m_Size.width, m_Size.height};
    LunaraEngine::BatchRenderer::AddQuad(rect, m_PlayerColor);
}

void Player::Move(f32 x, f32 y, f32 z, f32 delta)
{
    m_Position.x += (x * m_Speed.x * delta);
    m_Position.y += (y * m_Speed.y * delta);
    m_Position.z += (z * m_Speed.z * delta);
}

void Player::SetPlayerColor(LunaraEngine::Color4 color) { m_PlayerColor = color; }
void Player::SetPosition(const glm::vec3& position)
{
    m_Position = position;
}