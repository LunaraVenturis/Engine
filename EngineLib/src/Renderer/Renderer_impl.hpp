/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * Renderer class implementation
 */

#include "Renderer.hpp"
#include "RendererContext.hpp"
#include <Core/Log.hpp>

template <typename RendererContextType>
Engine::Renderer<RendererContextType>* Engine::Renderer<RendererContextType>::s_Renderer = nullptr;

namespace Engine
{
    template <typename RendererContextType>
    Renderer<RendererContextType>::Renderer(RendererSpec& rendererSpec)
    {
        Init(rendererSpec);
    }

    template <typename RendererContextType>
    Renderer<RendererContextType>::~Renderer()
    {
        LOG_INFO("Destroyed Renderer\n");
    }

    template <typename RendererContextType>
    ResultValueType<RendererStatus> Renderer<RendererContextType>::Init(RendererSpec& rendererSpec)
    {
        auto result = RendererContextType::Create(rendererSpec);

        uint32_t tries = 0, maxTries = 3;
        while (result != RendererContextStatus::Initialized && tries < maxTries)
        {
            result = RendererContextType::Create(rendererSpec);
            tries++;
        }

        if (result == RendererContextStatus::Initialized)
        {
            LOG_INFO("Created Renderer\n");
            return ResultValueType{RendererStatus::Initialized};
        }
        else { return ResultValueType{RendererStatus::Fail}; }
    }

    template <typename RendererContextType>
    ResultValueType<RendererStatus> Renderer<RendererContextType>::Create(RendererSpec& rendererSpec)
    {
        if (nullptr == Renderer::GetRenderer())
        {
            Renderer::s_Renderer = new Renderer();
            return Renderer::s_Renderer->Init(rendererSpec);
        }
        return ResultValueType{RendererStatus::Fail};
    }

    template <typename RendererContextType>
    ResultValueType<RendererStatus> Renderer<RendererContextType>::DeInit()
    {
        if (nullptr != Renderer::GetRenderer())
        {
            auto result = RendererContextType::Destroy();
            uint32_t tries = 0, maxTries = 3;
            while (result != RendererContextStatus::Destroyed && tries < maxTries)
            {
                result = RendererContextType::Destroy();
                tries++;
            }
            if (result == RendererContextStatus::Destroyed)
            {
                delete Renderer::s_Renderer;
                return ResultValueType{RendererStatus::Destroyed};
            }

            return ResultValueType{RendererStatus::Fail};
        }
        return ResultValueType{RendererStatus::Fail};
    }

    template <typename RendererContextType>
    ResultValueType<RendererStatus> Renderer<RendererContextType>::Destroy()
    {
        if (nullptr != Renderer::GetRenderer()) { return Renderer::GetRenderer()->DeInit(); }
        return ResultValueType{RendererStatus::Fail};
    }

    template <typename RendererContextType>
    RendererSpec Renderer<RendererContextType>::GetRendererSpec()
    {
        return Renderer::s_Renderer->m_RendererSpec;
    }

    template <typename RendererContextType>
    Renderer<RendererContextType>* Renderer<RendererContextType>::GetRenderer()
    {
        return Renderer::s_Renderer;
    }
}// namespace Engine