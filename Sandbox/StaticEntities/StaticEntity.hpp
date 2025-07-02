#pragma once
#include "glm/glm.hpp"
#include "UserTypes/UserTypes.hpp"
struct StaticEntitySize {
    f32 width;
    f32 height;
};

class StaticEntity 
{
    public:
    StaticEntity() = default;
    StaticEntity(glm::vec3 Position, StaticEntitySize Size);
    StaticEntity(const StaticEntity& other) = delete;
    StaticEntity(StaticEntity&& other) = delete;
    StaticEntity& operator=(const StaticEntity& other) = delete;
    StaticEntity& operator=(StaticEntity&& other) = delete;
    ~StaticEntity() = default;
    public:
    void Draw();
    private:
    glm::vec3 m_Position;
    StaticEntitySize m_Size;
};