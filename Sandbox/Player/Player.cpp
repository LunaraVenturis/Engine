#include "Player.hpp"
#include "Renderer/Renderer.hpp"

Player::Player(glm::vec2 playerPosition, PlayerSize playerSize) : m_Position(playerPosition), m_size(playerSize) {}

void Player::Init()
{
    std::vector<Vertex> vertices = {{glm::vec2{-0.10f, -0.10f}, glm::vec3{1.0f, 1.0f, 1.0f}},
                                    {glm::vec2{0.10f, -0.10f}, glm::vec3{1.0f, 1.0f, 1.0f}},
                                    {glm::vec2{0.10f, 0.10f}, glm::vec3{1.0f, 1.0f, 1.0f}},
                                    {glm::vec2{-0.10f, 0.10f}, glm::vec3{1.0f, 1.0f, 1.0f}}};
    m_QuadBuffer.Create(
            {{"Position", LunaraEngine::VertexAttributeType::VEC3}, {"Color", LunaraEngine::VertexAttributeType::VEC3}},
            (uint8_t*) vertices.data(), vertices.size());
    std::vector<uint16_t> indices{0, 1, 2, 2, 3, 0};
    m_QuadIndexBuffer.Create(indices.data(), indices.size());
}

void Player::Draw() { LunaraEngine::Renderer::DrawIndexed(&m_QuadBuffer, &m_QuadIndexBuffer); }

Player::~Player() {}