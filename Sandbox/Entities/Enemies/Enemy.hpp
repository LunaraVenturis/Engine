#pragma once
#include "Entities/Entity/Entity.hpp"
class Enemy: public Entity
{
public:
    Enemy() = delete;
    Enemy(glm::vec3 enemyPosition, EntitySize enemySize);
    Enemy(const Enemy& player) = delete;
    Enemy(Enemy&& player) = delete;
    Enemy& operator=(const Enemy&) = delete;
    Enemy& operator=(Enemy&&) = delete;
    ~Enemy() = default;

public:
    void Draw() override;
    void Move(f32 x, f32 y, f32 z, f32 delta) override;
    bool HasReachedPointX(f32 currentX, f32 destX);

    [[nodiscard]] glm::vec3 GetPosition() const override { return m_Position; };

    [[nodiscard]] EntitySize GetSize() const override { return m_size; };

private:
    glm::vec3 m_Position;
    glm::vec3 m_Speed;
    EntitySize m_size;
};