#pragma once
#include <LunaraEngine/Core/Events.hpp>
#include <LunaraEngine/Core/CommonTypes.hpp>
#include <string_view>
#include <memory>
#include <filesystem>

namespace LunaraEngine
{
    class Layer
    {
    public:
        bool ShouldExit() { return p_ShouldExit; }

        void SetShouldExit(bool value) { p_ShouldExit = value; }

    public:
        virtual ~Layer() noexcept(false) = default;

    public:
        virtual void Init(const ApplicationConfig& config) = 0;

        virtual void OnAttach() = 0;

        virtual void OnUpdate(float dt) = 0;

        virtual void OnDetach() = 0;

        virtual void Destroy() = 0;

        virtual void OnImGuiDraw() = 0;

        virtual void OnMouseMoveEvent(uint32_t width, uint32_t height) = 0;

        virtual void OnKeyboardEvent(uint32_t key, KeyEventType type) = 0;

        virtual void OnMouseButtonEvent(float x, float y, MouseButtonEventType type, MouseButton button) = 0;

        virtual void OnWindowResizeEvent(uint32_t width, uint32_t height) = 0;

        virtual void OnWindowShouldCloseEvent() = 0;

        virtual void Begin() = 0;

        virtual void End() = 0;

    protected:
        bool p_ShouldExit{};
    };
}// namespace LunaraEngine