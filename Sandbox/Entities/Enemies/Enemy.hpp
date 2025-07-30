#pragma once
#include <Entities/Entity/Entity.hpp>

class Enemy: public AnimatedEntity
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
    void UpdateAnimation(f32 delta) override;
    void Draw(std::weak_ptr<LunaraEngine::BatchRenderer> renderer) override;
    void Move(f32 x, f32 y, f32 z, f32 delta) override;
    bool HasReachedPointX(f32 currentX, f32 destX);

    [[nodiscard]] glm::vec3 GetPosition() const override { return m_Position; };

    [[nodiscard]] EntitySize GetSize() const override { return m_size; };

private:
    glm::vec3 m_Position;
    glm::vec3 m_Speed;
    EntitySize m_size;
};