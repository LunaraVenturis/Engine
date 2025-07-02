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

bool Player::isColliding(Entity* enemy)
{
    const float Z_THRESHOLD = 0.1f;
    return (m_Position.x < enemy->GetPosition().x + enemy->GetSize().width &&
            m_Position.x + m_Size.width > enemy->GetPosition().x &&
            m_Position.y < enemy->GetPosition().y + enemy->GetSize().height &&
            m_Position.y + m_Size.height > enemy->GetPosition().y) &&
           fabs(m_Position.z - enemy->GetPosition().z) < Z_THRESHOLD;
}

void Player::SetPlayerColor(LunaraEngine::Color4 color) { m_PlayerColor = color; }
