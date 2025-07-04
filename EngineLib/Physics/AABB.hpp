#pragma once
#include "UserTypes/UserTypes.hpp"
#include "glm/glm.hpp"

class AABB
{
public:
    AABB() = delete;
    AABB(const AABB& other) = delete;
    AABB(AABB&& other) = delete;
    AABB& operator=(const AABB& other) = delete;
    AABB& operator=(AABB&& other) = delete;

public:
    bool Intersection();
private:
    glm::vec3 m_Position;
    glm::vec3 m_HalfSize;
};