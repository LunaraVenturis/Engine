#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <Engine.hpp>

namespace LunaraEngine
{
    Camera::Camera(glm::vec2 screen) : m_Zoom(1.0f), m_Position({0, 0})
    {
        m_View = glm::mat4(1.0f);
        m_Model = glm::mat4(1.0f);
        OnResize((u32) screen.x, (u32) screen.y);
        CalculateView();
    }

    void Camera::OnUpdate(float dt) { (void) dt; }

    void Camera::OnResize(uint32_t width, uint32_t height)
    {
        m_ScreenSize = {width, height};
        m_AspectRatio = m_ScreenSize.x / m_ScreenSize.y;
        CalculateProjection();
    }

    void Camera::CalculateProjection()
    {
        m_Projection = glm::ortho(0.0f, m_ScreenSize.x * m_Zoom, 0.0f, m_ScreenSize.y * m_Zoom, -1.0f, 1.0f);
    }

    void Camera::CalculateView() { m_View = glm::translate(glm::mat4(1.0f), -glm::vec3{m_Position, 0.0f}); }

    void Camera::Move(glm::vec2 relativePosition)
    {
        m_Position += relativePosition;
        CalculateView();
    }

    void Camera::SetPosition(glm::vec2 position)
    {
        m_Position = position;
        CalculateView();
    }

    void Camera::SetZoom(f32 zoom)
    {
        m_Zoom = zoom;
        CalculateProjection();
    }

    void Camera::Zoom(f32 zoom)
    {
        m_Zoom += zoom;
        CalculateProjection();
    }

    void Camera::Upload(Shader* shader)
    {
        shader->SetUniform("projection", m_Projection);
        shader->SetUniform("view", m_View);
        shader->SetUniform("model", m_Model);
    }
}// namespace LunaraEngine