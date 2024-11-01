#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @date 
 *
 * @section DESCRIPTION
 * 
 * LayerStack class implementation
 */


#include <Layer/LayerStack.hpp>
#include <Core/Log.hpp>

Engine::LayerStack* Engine::LayerStack::s_LayerStack = nullptr;

namespace Engine
{
    inline ResultValueType<LayerStackStatus> LayerStack::Init()
    {
        if (LayerStack::s_LayerStack) { return ResultValueType(LayerStackStatus::Error); }

        s_LayerStack = new LayerStack();
        return ResultValueType(LayerStackStatus::Initialized);
    }

    template <typename T>
    inline ResultValueType<LayerStatus> LayerStack::AddLayer()
    {
        if (!LayerStack::s_LayerStack) { return ResultValueType(LayerStatus::Error); }

        LayerStack::s_LayerStack->m_Layers.emplace_back(new T());
        LOG_INFO("Layer %s added!\n", LayerStack::s_LayerStack->m_Layers.back()->GetName().data());

        LayerStack::s_LayerStack->m_Layers.back()->OnAttach();
        LOG_INFO("Layer %s attached!\n", LayerStack::s_LayerStack->m_Layers.back()->GetName().data());

        return {LayerStatus::Added};
    }

    ResultValueType<LayerStatus> LayerStack::RemoveLayer(std::string_view name)
    {
        if (!LayerStack::s_LayerStack) { return ResultValueType(LayerStatus::Error); }

        uint32_t index = 0;
        for (auto& layer: LayerStack::s_LayerStack->m_Layers)
        {
            if (layer->GetName() == std::string(name))
            {
                delete layer;
                layer->OnDettach();
                layer->OnDestroy();
                LayerStack::s_LayerStack->m_Layers.erase(LayerStack::s_LayerStack->m_Layers.begin() + index);
                break;
            }
            index++;
        }
        return ResultValueType(LayerStatus::Removed);
    }

    inline ResultValueType<LayerStackStatus> LayerStack::Destroy()
    {
        if (!LayerStack::s_LayerStack) { return ResultValueType(LayerStackStatus::Error); }

        auto status = LayerStack::DestroyLayers();
        if (LayerStackStatus::Destroyed != status) { return ResultValueType(LayerStackStatus::Error); }

        for (auto& layer: LayerStack::s_LayerStack->m_Layers)
        {
            LOG_INFO("Layer %s removed!\n", layer->GetName().data());
            delete layer;
        }
        LayerStack::s_LayerStack->m_Layers.clear();
        delete LayerStack::s_LayerStack;
        LayerStack::s_LayerStack = nullptr;

        return ResultValueType(LayerStackStatus::Destroyed);
    }

    inline ResultValue<LayerStackStatus, LayerStack*> LayerStack::Get()
    {
        if (!LayerStack::s_LayerStack) { return ResultValue(LayerStackStatus::Error, (LayerStack*) nullptr); }

        return ResultValue(LayerStackStatus::Success, s_LayerStack);
    }

    ResultValue<LayerStatus, Layer*> LayerStack::GetLayer(std::string_view name)
    {
        if (!LayerStack::s_LayerStack) { return ResultValue(LayerStatus::Error, (Layer*) nullptr); }

        for (auto& layer: LayerStack::s_LayerStack->m_Layers)
        {
            if (layer->GetName() == std::string(name)) { return ResultValue(LayerStatus::Success, layer); }
        }
        return ResultValue(LayerStatus::Error, (Layer*) nullptr);
    }

    inline ResultValue<LayerStatus, std::vector<Layer*>*> LayerStack::GetLayers()
    {
        if (!LayerStack::s_LayerStack) { return ResultValue(LayerStatus::Error, (std::vector<Layer*>*) nullptr); }

        return ResultValue<LayerStatus, std::vector<Layer*>*>(LayerStatus::Success,
                                                              &LayerStack::s_LayerStack->m_Layers);
    }

    inline ResultValueType<LayerStackStatus> LayerStack::InitLayers()
    {
        if (!LayerStack::s_LayerStack) { return ResultValueType(LayerStackStatus::Error); }

        for (auto& layer: LayerStack::s_LayerStack->m_Layers)
        {
            layer->Init();
            LOG_INFO("Layer %s initialized!", LayerStack::s_LayerStack->m_Layers.back()->GetName().data());
        }
        return ResultValueType(LayerStackStatus::Initialized);
    }

    inline ResultValueType<LayerStackStatus> LayerStack::DestroyLayers()
    {
        if (!LayerStack::s_LayerStack) { return ResultValueType(LayerStackStatus::Error); }
        for (auto& layer: LayerStack::s_LayerStack->m_Layers)
        {
            layer->Destroy();
            LOG_INFO("Layer %s destroyed!\n", LayerStack::s_LayerStack->m_Layers.back()->GetName().data());

            layer->OnDettach();
            LOG_INFO("Layer %s dettached!\n", LayerStack::s_LayerStack->m_Layers.back()->GetName().data());
        }
        return ResultValueType(LayerStackStatus::Destroyed);
    }

}// namespace Engine