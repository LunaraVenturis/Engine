#pragma once
#include "glm/glm.hpp"
#include "UserTypes/UserTypes.hpp"

struct WallSize {
    f32 width;
    f32 height;
};

class Wall
{
public:
    Wall() = default;
    Wall(glm::vec3 Position, WallSize Size);
    Wall(const Wall& other) = delete;
    Wall(Wall&& other) = delete;
    Wall& operator=(const Wall& other) = delete;
    Wall& operator=(Wall&& other) = delete;
    ~Wall() = default;

public:
    void Draw();

private:
    glm::vec3 m_Position;
    WallSize m_Size;
};