#pragma once
#include "Entities/Entity/Entity.hpp"
#include <LunaraEngine/Engine.hpp>
#include <LunaraEngine/Math/Color.h>

class Coin: public AnimatedEntity
{
public:
    Coin() = delete;
    Coin(const glm::vec3& coinPosition, const EntitySize coinSize);
    ~Coin() = default;

public:
    void UpdateAnimation(f32 delta) override;
    void Draw(std::weak_ptr<LunaraEngine::BatchRenderer> renderer) override;
    void Move(f32 x, f32 y, f32 z, f32 delta) override;
    bool isColliding(Entity* entity);
    void SetPosition(const glm::vec3& position);

    [[nodiscard]] glm::vec3 GetPosition() const override { return m_Position; };

    [[nodiscard]] EntitySize GetEntity() const override { return m_Size; };

private:
    glm::vec3 m_Position;
    glm::vec3 m_Speed;
    EntitySize m_Size;
};
