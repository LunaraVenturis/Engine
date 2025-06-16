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
        void OnUpdate();

    private:
        f32 m_AspectRatio;
        glm::vec3 m_Position;
        glm::mat4 m_Projection;
        glm::mat4 m_View;
        glm::mat4 m_Model;
        glm::mat4 m_MVP;
    };
}// namespace LunaraEngine