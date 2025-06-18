#pragma once
#include "glm/glm.hpp"
#include <vector>
#include "UserTypes/UserTypes.hpp"


struct EntitySize {
    f32 width;
    f32 height;
};

class Entity
{
public:
    Entity() = default;
    Entity(const Entity& player) = delete;
    Entity(Entity&& player) = delete;
    Entity& operator=(const Entity&) = delete;
    Entity& operator=(Entity&&) = delete;
    virtual ~Entity() = default;

public:
   virtual void Draw() = 0;
   virtual void Move(f32 x, f32 y, f32 delta) = 0;
   [[nodiscard]] virtual glm::vec2 GetPosition() const = 0;
   [[nodiscard]] virtual EntitySize GetSize() const = 0;

};