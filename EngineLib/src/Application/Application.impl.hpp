#pragma once
/**
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @date 31.10.2024
 * 
 * @section DESCRIPTION
 * 
 * Application class implementation
 */

#include <Layer/LayerStack.hpp>
#include <Core/Log.hpp>
#include <Renderer/Renderer.hpp>

#include "Application.hpp"
#include <concepts>

Engine::Application* Engine::Application::s_Application = nullptr;

namespace Engine
{

    void Application::Run()
    {
        LayerStack::InitLayers();

        while (!Application::Get()->m_StoppedFlag)
        {
            auto& layersStatus = *LayerStack::GetLayers().value;
            for (auto& layer: layersStatus)
            {
                layer->OnUpdate();
                Application::Get()->m_StoppedFlag = true;
            }
        }
    }

    void Application::Init(ApplicationSpec applicationSpec)
    {
        if (nullptr == Application::s_Application)
        {
            Application::s_Application = new Application();
            Application::s_Application->m_ApplicationSpec = applicationSpec;

            LOG_INFO("Application initialized!\n");
            RendererSpec rendererSpec;
            rendererSpec.AppName = applicationSpec.ApplicationName;
            rendererSpec.WorkingDirectory = Application::GetSpec().WorkingDirectory;
            rendererSpec.width = Application::GetSpec().StartupWidth;
            rendererSpec.height = Application::GetSpec().StartupHeight;
            Renderer<>::Create(rendererSpec);

            LayerStack::Init();
        }
    }

    void Application::Destroy()
    {

        if (nullptr != Application::s_Application)
        {
            LayerStack::Destroy();

            Renderer<>::Destroy();

            delete Application::s_Application;
            LOG_INFO("Application destroyed!\n");
        }
    }

    Application* Application::Get() { return Application::s_Application; }

    ApplicationSpec& Application::GetSpec() { return Application::Get()->m_ApplicationSpec; }

    template <typename T>
    void Application::AddLayer()
    {
        if (!std::derived_from<T, Layer>) { LOG_INFO("Layer type does not have Layer as a Base type!\n"); }
        else { LayerStack::AddLayer<T>(); }
    }
}// namespace Engine