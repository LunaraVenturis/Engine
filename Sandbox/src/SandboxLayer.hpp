#pragma once
#include <Layer/Layer.hpp>
#include <Engine.hpp>

class SandboxLayer: public Engine::Layer
{
public:
    SandboxLayer() { p_Name = "Sandbox Layer"; };

    virtual void Init() override
    {

        uint8_t data[] = {1, 3, 5, 7, 9, 11, 13, 15};
        Engine::Buffer b(data, sizeof(data));
        Engine::Buffer c = Engine::Buffer::Copy(data, sizeof(data));
    }

    void Destroy() override {}

    void OnAttach() override {}

    void OnDettach() override {}

    void OnDestroy() override {}

    void OnUpdate() override {}

    void OnMouseClickEvent() override {}

    void OnMouseMoveEvent() override {}

    void OnKeyboardEvent() override {}
};