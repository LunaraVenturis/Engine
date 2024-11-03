/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * RendererContext class implementation
 */

#include "RendererContext.hpp"
#include "VulkanContext.hpp"
#include <Core/Log.hpp>
#include <GLFW/glfw3.h>

Engine::RendererContext* Engine::RendererContext::s_RendererContext = nullptr;

namespace Engine
{
    RendererContext::RendererContext() {}

    RendererContext::RendererContext(RendererSpec& rendererSpec) { Init(rendererSpec); }

    RendererContext::~RendererContext() { LOG_INFO("Destroyed RendererContext\n"); }

    ResultValueType<RendererContextStatus> RendererContext::Init(RendererSpec& rendererSpec)
    {
        glfwInit();
        LOG_INFO("Creating RendererContext!\n");
        auto result = Window::Create(rendererSpec);
        uint32_t tries = 0, maxTries = 3;
        while (result.status != WindowStatus::Created && tries < maxTries)
        {
            result = Window::Create(rendererSpec);
            tries++;
        }

        auto vkContextResult =
                VulkanContext::Create(VulkanSpec{rendererSpec, result.value->GetRequiredExtensions().value}, result.value);
        if (VulkanContextStatus::Created != vkContextResult)
        {
            LOG_ERROR("Could not create Vulkan Context!\n");
            return ResultValueType{RendererContextStatus::Fail};
        }

        if (result == WindowStatus::Created)
        {
            m_Window = result.value;
            return ResultValueType{RendererContextStatus::Initialized};
        }
        return ResultValueType{RendererContextStatus::Fail};
    }

    ResultValueType<RendererContextStatus> RendererContext::Create(RendererSpec& rendererSpec)
    {
        if (nullptr == RendererContext::GetRenderer())
        {
            RendererContext::s_RendererContext = new RendererContext();
            auto result = RendererContext::s_RendererContext->Init(rendererSpec);

            uint32_t tries = 0, maxTries = 3;
            while (result != RendererContextStatus::Initialized && tries < maxTries)
            {
                result = RendererContext::s_RendererContext->Init(rendererSpec);
                tries++;
            }

            if (result == RendererContextStatus::Initialized)
            {
                return ResultValueType{RendererContextStatus::Initialized};
            }
            else { return ResultValueType{RendererContextStatus::Fail}; }
        }
        return ResultValueType{RendererContextStatus::Not_Initialized};
    }

    ResultValueType<RendererContextStatus> RendererContext::Destroy()
    {
        VulkanContext::Destroy();

        Window::Destroy(RendererContext::GetRenderer()->m_Window);
        glfwTerminate();
        if (nullptr != RendererContext::GetRenderer())
        {
            delete RendererContext::s_RendererContext;
            return ResultValueType{RendererContextStatus::Destroyed};
        }
        return ResultValueType{RendererContextStatus::Not_Initialized};
    }

    RendererSpec RendererContext::GetRendererSpec() { return RendererContext::s_RendererContext->m_RendererSpec; }

    RendererContext* RendererContext::GetRenderer() { return RendererContext::s_RendererContext; }
}// namespace Engine