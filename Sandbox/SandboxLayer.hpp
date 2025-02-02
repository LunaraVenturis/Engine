#pragma once

#include <Engine.hpp>

class SandboxLayer: public LunaraEngine::Layer
{

public:
    SandboxLayer() = default;
    ~SandboxLayer() = default;

public:
    virtual void Init() override;

    virtual void OnAttach() override {}

    virtual void OnUpdate(float dt) override;

    virtual void OnDetach() override {}

    virtual void Destroy() override {}

    virtual void OnImGuiDraw() override {}

    virtual void OnMouseMoveEvent(int width, int height) override
    {
        (void) width;
        (void) height;
    }

    virtual void OnKeyboardEvent(int action, int key) override
    {
        (void) action;
        (void) key;
    }

    virtual void OnWindowResizeEvent(int width, int height) override
    {
        (void) width;
        (void) height;
    }

    virtual void OnWindowShouldCloseEvent() override {}

    virtual void Begin() override {}

    virtual void End() override {}

private:
    Window* m_Window;
};