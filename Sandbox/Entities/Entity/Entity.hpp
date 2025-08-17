#pragma once
#include "glm/glm.hpp"
#include <LunaraEngine/Engine.hpp>

struct EntitySize {
    f32 width;
    f32 height;
};

class Entity
{
public:
    Entity() = default;
    virtual ~Entity() = default;

public:
    void SetSpriteSheetIndex(u32 index) { p_StartTextureIndex = index; }

public:
    virtual void Draw(std::weak_ptr<LunaraEngine::BatchRenderer> renderer) = 0;
    virtual void Move(f32 x, f32 y, f32 z, f32 delta) = 0;
    [[nodiscard]] virtual glm::vec3 GetPosition() const = 0;
    [[nodiscard]] virtual EntitySize GetEntity() const = 0;

protected:
    u32 p_StartTextureIndex{};
};

class AnimatedEntity: public Entity
{
public:
    virtual ~AnimatedEntity() = default;
    virtual void UpdateAnimation(f32 delta) = 0;

public:
    void SetAnimationFrameCount(u32 count) { p_MaxAnimationIndex = count; }

    void SetAnimationSpeed(f32 speed) { p_AnimationSpeed = speed; }

    void SetAnimationTimer(f32 timer) { p_AnimationTimer = timer; }

    void SetAnimationFactor(f32 factor) { p_AnimationFactor = factor; }

protected:
    u32 p_AnimationIndex{};
    u32 p_MaxAnimationIndex{};
    f32 p_AnimationTimer{};
    f32 p_AnimationSpeed{100.0};
    f32 p_AnimationFactor{10.0};
};