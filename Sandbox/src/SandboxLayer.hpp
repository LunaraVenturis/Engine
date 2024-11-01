#pragma once
#include <Layer/Layer.hpp>
#include <Engine.hpp>

class SandboxLayer: public Engine::Layer
{
public:
    SandboxLayer() = default;

    virtual void Init() override {}

    void Destroy() override {}

    void OnAttach() override {}

    void OnDettach() override {}

    void OnDestroy() override {}

    void OnUpdate() override {}

    void OnMouseClickEvent() override {}

    void OnMouseMoveEvent() override {}

    void OnKeyboardEvent() override {}
};