#pragma once
#include "glm/glm.hpp"
#include "Renderer/IndexBuffer.hpp"
#include "Renderer/VertexBuffer.hpp"
#include <vector>

using f32 = float;

struct PlayerSize {
    f32 width;
    f32 height;
};

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

class Player
{
public:
    Player() = delete;
    Player(glm::vec2 playerPosition, PlayerSize playerSize);
    Player(const Player& player) = delete;
    Player(Player&& player) = delete;
    ~Player();

public:
    void Draw();
    void Init();
    void MovePlayer(f32 x, f32 y, f32 delta);
    [[nodiscard]] glm::vec2 GetPlayerPosition() const { return m_Position; }
    [[nodiscard]] PlayerSize GetPlayerSize() const { return m_size; }

private:
    glm::vec2 m_Position;
    glm::vec2 m_Speed;
    PlayerSize m_size;
    LunaraEngine::VertexBuffer m_QuadBuffer;
    LunaraEngine::IndexBuffer<> m_QuadIndexBuffer;
};