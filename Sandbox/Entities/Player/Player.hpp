#pragma once
#include "Entities/Entity/Entity.hpp"
#include <LunaraEngine/Engine.hpp>
#include <LunaraEngine/Math/Color.h>



class Player: public AnimatedEntity
{
public:

    Player(const glm::vec3& playerPosition, const EntitySize playerSize, const FacingDirection direction);
    ~Player() = default;

public:
    void UpdateAnimation(f32 delta) override;
    void Draw(std::weak_ptr<LunaraEngine::BatchRenderer> renderer) override;
    void Move(f32 x, f32 y, f32 z, f32 delta) override;
    bool isColliding(Entity* Enemy);
    void SetPosition(const glm::vec3& position);
    void SetDirection(const FacingDirection);
    [[nodiscard]] glm::vec3 GetPosition() const override { return m_Position; };
    [[nodiscard]] u32 GetDirection() const override { return m_Dir;}
    [[nodiscard]] EntitySize GetEntity() const override { return m_Size; };

private:
    glm::vec3 m_Position;
    glm::vec3 m_Speed;
    EntitySize m_Size;
    u32 m_Dir;
};
