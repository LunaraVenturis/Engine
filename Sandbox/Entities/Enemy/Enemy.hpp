#include "Entities/Entity/Entity.hpp"

#pragma once
#include "Entities/Entity/Entity.hpp"
class Enemy: public Entity
{
public:
    Enemy() = delete;
    Enemy(glm::vec2 playerPosition, EntitySize playerSize);
    Enemy(const Enemy& player) = delete;
    Enemy(Enemy&& player) = delete;
    Enemy& operator=(const Enemy&) = delete;
    Enemy& operator=(Enemy&&) = delete;
    ~Enemy() = default;

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