#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 

 * @section DESCRIPTION
 * 
 * Renderer class definition
 */

#include "RendererSpec.hpp"
#include <Renderer/RendererContext.hpp>
#include <Core/Result.hpp>
#include <filesystem>

namespace Engine
{
    enum class RendererStatus
    {
        Success,
        Fail,
        Created,
        Initialized,
        Not_Initialized,
        Destroyed
    };

    template <typename RendererContextType = Engine::RendererContext>
    class Renderer
    {
    public:
        Renderer() = default;
        Renderer(RendererSpec& rendererSpec);
        ~Renderer();

    public:
        ResultValueType<RendererStatus> Init(RendererSpec& RendererSpec);
        ResultValueType<RendererStatus> DeInit();

    public:
        static ResultValueType<RendererStatus> Create(RendererSpec& rendererSpec);

        static ResultValueType<RendererStatus> Destroy();

        static Renderer* GetRenderer();

        static RendererSpec GetRendererSpec();

    private:
        static Renderer* s_Renderer;

    private:
        RendererSpec m_RendererSpec;
    };
}// namespace Engine

#include "Renderer_impl.hpp"