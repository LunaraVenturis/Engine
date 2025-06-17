#pragma once
#include "Renderer/Shader.hpp"
#include "glm/glm.hpp"
using f32 = float;

namespace LunaraEngine
{
    class Shader;

    class Camera
    {
    public:
        Camera(glm::vec2 screen);
        ~Camera() = default;

    public:
        [[nodiscard]] f32 GetAspectRatio() const { return m_AspectRatio; }

        [[nodiscard]] glm::vec3 GetPosition() const { return m_Position; }

        [[nodiscard]] glm::mat4 GetProjectionMatrix() const { return m_Projection; }

        [[nodiscard]] glm::mat4 GetViewMatrix() const { return m_View; }

        [[nodiscard]] glm::mat4 GetModelMatrix() const { return m_Model; }

    public:
        void Upload(Shader* shader);
        void Init();
        void OnUpdate(float dt);
        void OnResize(uint32_t width, uint32_t height);
        [[nodiscard]] void Move(glm::vec2 relativePosition);
        [[nodiscard]] void SetPosition(glm::vec2 position);

    private:
        f32 m_AspectRatio;
        glm::vec2 m_Position;
        glm::mat4 m_Projection;
        glm::mat4 m_View;
        glm::mat4 m_Model;
    };
}// namespace LunaraEngine