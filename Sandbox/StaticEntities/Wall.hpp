#pragma once
#include <glm/glm.hpp>
#include <LunaraEngine/Core/CommonTypes.hpp>
#include <Entities/Entity/Entity.hpp>

struct WallSize {
    f32 width;
    f32 height;
};

class Wall
{
public:
    Wall(const glm::vec3& Position, WallSize Size);
    ~Wall() = default;

public:


    [[nodiscard]] glm::vec3 GetPosition() const { return m_Position; }

    [[nodiscard]] WallSize GetWallSize() const { return m_Size; }

public:
    void SetSpriteSheetIndex(u32 index) { m_StartTextureIndex = index; }

    void Draw(std::weak_ptr<LunaraEngine::BatchRenderer> renderer);

private:
    u32 m_StartTextureIndex{};
    glm::vec3 m_Position;
    WallSize m_Size;
};