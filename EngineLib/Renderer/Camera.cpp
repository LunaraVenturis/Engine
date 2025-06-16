#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace LunaraEngine
{
    Camera::Camera(glm::vec2 screen) : m_AspectRatio(static_cast<f32>(screen.x) / screen.y), m_Position({0, 0, 0})
    {
        m_Projection = glm::ortho(0.0f, screen.x, 0.0f, screen.y, -1.0f, 1.0f);
        m_View = glm::mat4(1.0f);
        m_Model = glm::mat4(1.0f);
    }
    void Camera::OnUpdate() {}

    void Camera::Upload(Shader* shader)
    {
        m_View = glm::translate(glm::mat4(1.0f), -m_Position);
        m_MVP =  m_Projection * m_View * m_Model;
        shader->SetUniform("mvp", m_MVP);
    }
}// namespace LunaraEngine