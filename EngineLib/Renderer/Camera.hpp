#pragma once
#include "glm/glm.hpp"

namespace LunaraEngine
{
    class Shader;

    class Camera
    {
        using f32 = float;
        using u32 = uint32_t;

    public:
        Camera(glm::vec2 screen);
        ~Camera() = default;

    public:
        [[nodiscard]] f32 GetAspectRatio() const { return m_AspectRatio; }

        [[nodiscard]] glm::vec2 GetPosition() const { return m_Position; }

        [[nodiscard]] glm::mat4 GetProjectionMatrix() const { return m_Projection; }

        [[nodiscard]] glm::mat4 GetViewMatrix() const { return m_View; }

        [[nodiscard]] glm::mat4 GetModelMatrix() const { return m_Model; }

    public:
        void Upload(Shader* shader);
        void Init();
        void OnUpdate(float dt);
        void OnResize(uint32_t width, uint32_t height);
        void Move(glm::vec2 relativePosition);
        void SetPosition(glm::vec2 position);
        void SetZoom(f32 zoom);
        void Zoom(f32 zoom);

    private:
        void CalculateProjection();
        void CalculateView();

    private:
        f32 m_AspectRatio;
        f32 m_Zoom;
        glm::vec2 m_ScreenSize;
        glm::vec2 m_Position;
        glm::mat4 m_Projection;
        glm::mat4 m_View;
        glm::mat4 m_Model;
    };
}// namespace LunaraEngine