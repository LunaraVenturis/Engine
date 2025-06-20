#include "Player.hpp"
#include <Engine.hpp>

Player::Player(glm::vec2 playerPosition, PlayerSize playerSize)
    : m_Position(playerPosition), m_Speed({2.0f, 2.0f}), m_size(playerSize)
{}

void Player::Init() {}

void Player::Draw()
{
    auto rect = LunaraEngine::FRect{m_Position.x, m_Position.y, m_size.width, m_size.height};
    auto color = LunaraEngine::Color4{1.0f, 1.0f, 1.0f, 1.0f};
    LunaraEngine::BatchRenderer::AddQuad(rect, color);
}

void Player::MovePlayer(f32 x, f32 y, f32 delta)
{
    m_Position.x += (x * m_Speed.x * delta);
    m_Position.y += (y * m_Speed.y * delta);
}

Player::~Player() {}