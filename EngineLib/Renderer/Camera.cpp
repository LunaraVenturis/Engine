#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace LunaraEngine
{
    Camera::Camera(glm::vec2 screen) : m_Position({0, 0})
    {
        OnResize(screen.x, screen.y);
        m_View = glm::mat4(1.0f);
        m_Model = glm::mat4(1.0f);
    }

    void Camera::OnUpdate(float dt) {}

    void Camera::OnResize(uint32_t width, uint32_t height)
    {
        m_AspectRatio = static_cast<f32>(width) / height;
        m_Projection = glm::ortho(0.0f, static_cast<f32>(width), 0.0f, static_cast<f32>(height), -1.0f, 1.0f);
    }

    void Camera::Move(glm::vec2 relativePosition)
    {
        m_Position += relativePosition;
        m_View = glm::translate(glm::mat4(1.0f), -glm::vec3{m_Position, 0.0f});
    }

    void Camera::SetPosition(glm::vec2 position)
    {
        m_Position = position;
        m_View = glm::translate(glm::mat4(1.0f), -glm::vec3{m_Position, 0.0f});
    }

    void Camera::Upload(Shader* shader)
    {
        glm::mat4 mvp = m_Projection * m_View * m_Model;
        shader->SetUniform("mvp", mvp);
    }
}// namespace LunaraEngine