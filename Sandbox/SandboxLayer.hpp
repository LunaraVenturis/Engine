#pragma once

#include <Engine.hpp>
#include <Entities/Player/Player.hpp>
#include <Entities/Enemies/Enemy.hpp>
#include <StaticEntities/Wall.hpp>
#include "Renderer/Camera.hpp"

#include <vector>

class SandboxLayer: public LunaraEngine::Layer
{

public:
    SandboxLayer() = default;
    ~SandboxLayer() = default;

public:
    virtual void Init(std::filesystem::path workingDirectory) override;

    virtual void OnAttach() override {}

    virtual void OnUpdate(float dt) override;

    virtual void OnDetach() override {}

    virtual void Destroy() override;

    virtual void OnImGuiDraw() override {}

    virtual void OnMouseButtonEvent(float x, float y, MouseButtonEventType type, MouseButton button) override
    {
        (void) x;
        (void) y;
        (void) type;
        (void) button;
    }

    virtual void OnMouseMoveEvent(uint32_t width, uint32_t height) override;

    virtual void OnKeyboardEvent(uint32_t key, KeyEventType type) override;

    virtual void OnWindowResizeEvent(uint32_t width, uint32_t height) override;

    virtual void OnWindowShouldCloseEvent() override {}

    virtual void Begin() override {}

    virtual void End() override {}

private:
    LunaraEngine::Window* m_Window;
    LunaraEngine::Font m_Font;
    uint32_t x{}, y{};

    Player m_Player = {{0.0f, 0.0f, 0.0f}, {50.0f, 50.0f}};
    Enemy m_Enemy = {{0.0f, 0.0f, 0.0f}, {50.0f, 50.0f}};
    Wall m_Wall = {{250.0f, 250.0f, 0.0f}, {100.0f, 50.0f}};
    LunaraEngine::Camera m_Camera = {{1280.0f, 720.0f, 0}};
    LunaraEngine::AABB m_PlayerCollider;
    float zoom{1.0f};
    std::map<uint32_t, uint8_t> m_PressedKeys{};

    std::shared_ptr<LunaraEngine::Shader> m_BatchQuadShader;
    float elapsedTime{};
    float m_PlayerDt{};
    f32 dir = 1.0f;
};