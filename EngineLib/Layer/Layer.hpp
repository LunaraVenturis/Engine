#pragma once

#include <string_view>
#include <memory>

namespace LunaraEngine
{
    class Layer
    {
    public:
        bool ShouldExit() { return p_ShouldExit; }

        void SetShouldExit(bool value) { p_ShouldExit = value; }

    public:
        virtual ~Layer() = default;

    public:
        virtual void Init() = 0;

        virtual void OnAttach() = 0;

        virtual void OnUpdate(float dt) = 0;

        virtual void OnDetach() = 0;

        virtual void Destroy() = 0;

        virtual void OnImGuiDraw() = 0;

        virtual void OnMouseMoveEvent(int width, int height) = 0;

        virtual void OnKeyboardEvent(int action, int key) = 0;

        virtual void OnWindowResizeEvent(int width, int height) = 0;

        virtual void OnWindowShouldCloseEvent() = 0;

        virtual void Begin() = 0;

        virtual void End() = 0;

    protected:
        bool p_ShouldExit{};
    };
}// namespace LunaraEngine