#pragma once
#include "Layer.hpp"
#include <LunaraEngine/Core/Events.hpp>
#include <LunaraEngine/Core/CommonTypes.hpp>
#include <unordered_map>
#include <vector>
#include <utility>
#include <string>
#include <filesystem>

namespace LunaraEngine
{

    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack() = default;

    public:
        template <typename T, typename... Args>
        static void PushLayer(std::string_view name, Args&&... args)
        {
            Layer* layer = new T(std::forward<Args>(args)...);
            LayerStack::_PushLayer(layer, name);
        }

        static void PopLayer(std::string_view name);

        static void PopLayers();

        static void InitLayers(const ApplicationConfig& config);

        static void DestroyLayers();

        static Layer* GetLayer(std::string_view name);

        static auto end();

        static auto begin();

        static auto& data();

        static void Begin();

        static void End();

        static void OnUpdate(float dt);

        static void OnImGuiDraw();

        static void OnEvent(Event* event);

    private:
        static uint32_t _FindLayerIndex(std::string_view name);
        static bool _ContainsLayer(std::string_view name);
        static void _PushLayer(Layer* layer, std::string_view name = "");

    private:
        inline static std::vector<std::string_view> m_LayersKeys;
        inline static std::vector<Layer*> m_Layers;
    };
}// namespace LunaraEngine