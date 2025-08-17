#pragma once
#include <Entities/Entity/Entity.hpp>

class Enemy: public AnimatedEntity
{
public:
    Enemy() = delete;
    Enemy(const glm::vec3& enemyPosition, const EntityData& enemySize);
    ~Enemy() = default;

public:
    void UpdateAnimation(f32 delta) override;
    void Draw(std::weak_ptr<LunaraEngine::BatchRenderer> renderer) override;
    void Move(f32 x, f32 y, f32 z, f32 delta) override;
    bool HasReachedPointX(f32 currentX, f32 destX);

    [[nodiscard]] glm::vec3 GetPosition() const override { return m_Position; };

    [[nodiscard]] EntityData GetEntity() const override { return m_Data; };

private:
    glm::vec3 m_Position;
    glm::vec3 m_Speed;
    EntityData m_Data;
};