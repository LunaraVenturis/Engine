#include "Player.hpp"
#include <Engine.hpp>

Player::Player(glm::vec2 playerPosition, EntitySize playerSize)
    : m_Position(playerPosition), m_Speed({2.0f, 2.0f}), m_size(playerSize), m_PlayerColor({1.0f, 1.0f, 1.0f, 1.0f})
{}

void Player::Draw()
{
    auto rect = LunaraEngine::FRect{m_Position.x, m_Position.y, m_size.width, m_size.height};
    LunaraEngine::BatchRenderer::AddQuad(rect, m_PlayerColor);
}

void Player::Move(f32 x, f32 y, f32 delta)
{
    m_Position.x += (x * m_Speed.x * delta);
    m_Position.y += (y * m_Speed.y * delta);
}

bool Player::isColliding(Entity* enemy)
{
    return (m_Position.x < enemy->GetPosition().x + enemy->GetSize().width &&
            m_Position.x + m_size.width > enemy->GetPosition().x &&
            m_Position.y < enemy->GetPosition().y + enemy->GetSize().height &&
            m_Position.y + m_size.height > enemy->GetPosition().y);
}

void Player::SetPlayerColor(LunaraEngine::Color4 color)
{
    m_PlayerColor = color;
}