#include "LayerStack.hpp"

namespace LunaraEngine
{
    void LayerStack::InitLayers(const ApplicationConfig& config)
    {
        for (const auto& layer: m_Layers) { layer->Init(config); }
    }

    void LayerStack::_PushLayer(Layer* layer, std::string_view name)
    {
        if (!LayerStack::_ContainsLayer(name))
        {
            LayerStack::m_Layers.push_back(layer);
            LayerStack::m_LayersKeys.push_back(name);
            layer->OnAttach();
        }
    }

    void LayerStack::PopLayer(std::string_view name)
    {
        if (LayerStack::_ContainsLayer(name))
        {
            uint32_t index = _FindLayerIndex(name);
            LayerStack::m_Layers[index]->OnDetach();

            LayerStack::m_Layers.erase(LayerStack::m_Layers.begin() + index);
            LayerStack::m_LayersKeys.erase(LayerStack::m_LayersKeys.begin() + index);
        }
    }

    void LayerStack::PopLayers()
    {
        for (uint32_t index = 0; index < m_Layers.size(); ++index)
        {
            LayerStack::m_Layers[index]->OnDetach();
            LayerStack::m_Layers[index]->Destroy();
            delete LayerStack::m_Layers[index];
        }
        m_Layers.clear();
        m_LayersKeys.clear();
    }

    void LayerStack::DestroyLayers()
    {
        for (uint32_t index = 0; index < m_Layers.size(); ++index)
        {
            LayerStack::m_Layers[index]->OnDetach();
            LayerStack::m_Layers[index]->Destroy();
            delete LayerStack::m_Layers[index];
            LayerStack::m_Layers.erase(LayerStack::m_Layers.begin() + index);
            LayerStack::m_LayersKeys.erase(LayerStack::m_LayersKeys.begin() + index);
        }
    }

    Layer* LayerStack::GetLayer(std::string_view name) { return LayerStack::m_Layers[_FindLayerIndex(name)]; }

    auto LayerStack::end() { return LayerStack::m_Layers.end(); }

    auto LayerStack::begin() { return LayerStack::m_Layers.begin(); }

    auto& LayerStack::data() { return LayerStack::m_Layers; }

    void LayerStack::Begin()
    {
        for (auto& layer: LayerStack::m_Layers) { layer->Begin(); }
    }

    void LayerStack::End()
    {
        for (auto& layer: LayerStack::m_Layers) { layer->End(); }
    }

    void LayerStack::OnUpdate(float dt)
    {
        for (auto& layer: LayerStack::m_Layers) { layer->OnUpdate(dt); }
    }

    void LayerStack::OnImGuiDraw()
    {
        for (auto& layer: LayerStack::m_Layers) { layer->OnImGuiDraw(); }
    }

    void LayerStack::OnEvent(Event* event)
    {

        for (auto& layer: LayerStack::m_Layers)
        {
            switch (event->type)
            {
                case EVENT_KEYBOARD:
                    layer->OnKeyboardEvent(event->keyEvent.key, event->keyEvent.type);
                    break;
                case EVENT_MOUSE_MOTION:
                    layer->OnMouseMoveEvent((uint32_t) event->mouseMotionEvent.x, (uint32_t) event->mouseMotionEvent.y);
                    break;
                case EVENT_MOUSE_BUTTON:
                    layer->OnMouseButtonEvent(event->mouseButtonEvent.x, event->mouseButtonEvent.y,
                                              event->mouseButtonEvent.type, event->mouseButtonEvent.button);
                    break;
                case EVENT_RESIZE_WINDOW:
                    layer->OnWindowResizeEvent(
                            (uint32_t) event->resizeWindowEvent.width, (uint32_t) event->resizeWindowEvent.height);
                    break;
                case EVENT_QUIT:
                    layer->OnWindowShouldCloseEvent();
                    break;
                default:
                    break;
            }
        }
    }

    uint32_t LayerStack::_FindLayerIndex(std::string_view name)
    {

        for (uint32_t i = 0; i < m_LayersKeys.size(); i++)
        {
            if (m_LayersKeys[i] == name) { return i; }
        }
        return (uint32_t) LayerStack::m_Layers.size();
    }

    bool LayerStack::_ContainsLayer(std::string_view name)
    {
        if (_FindLayerIndex(name) != LayerStack::m_Layers.size()) { return true; }
        return false;
    }

}// namespace LunaraEngine