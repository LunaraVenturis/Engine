#pragma once
#include "Entities/Entity/Entity.hpp"
class Player: public Entity
{
public:
    Player() = delete;
    Player(glm::vec2 playerPosition, EntitySize playerSize);
    Player(const Player& player) = delete;
    Player(Player&& player) = delete;
    Player& operator=(const Player&) = delete;
    Player& operator=(Player&&) = delete;
    ~Player() = default;

public:
    void Draw() override;
    void Move(f32 x, f32 y, f32 delta) override;

    [[nodiscard]] glm::vec2 GetPosition() const override { return m_Position; };

    [[nodiscard]] EntitySize GetSize() const override { return m_size; };

private:
    glm::vec2 m_Position;
    glm::vec2 m_Speed;
    EntitySize m_size;
};