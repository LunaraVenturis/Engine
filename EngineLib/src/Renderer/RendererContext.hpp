#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * RendererContext class definition
 */

#include "RendererSpec.hpp"
#include "Window.hpp"
#include <Core/Result.hpp>

enum class RendererContextStatus
{
    Success,
    Fail,
    Created,
    Initialized,
    Not_Initialized,
    Destroyed
};

namespace Engine
{

    class RendererContext
    {
    public:
        RendererContext();
        RendererContext(RendererSpec& rendererSpec);
        ~RendererContext();

    public:
        ResultValueType<RendererContextStatus> Init(RendererSpec& rendererSpec);

    public:
        static ResultValueType<RendererContextStatus> Create(RendererSpec& rendererSpec);

        static ResultValueType<RendererContextStatus> Destroy();

        static RendererContext* GetRenderer();

        static RendererSpec GetRendererSpec();

    private:
        static RendererContext* s_RendererContext;

    private:
        Window* m_Window{};
        RendererSpec m_RendererSpec{};
    };
}// namespace Engine