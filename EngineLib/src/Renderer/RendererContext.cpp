/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section LICENSE
 * MIT License
 * 
 * Copyright (c) 2024 Christian and Krusto
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @section DESCRIPTION
 * 
 * RendererContext class implementation
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "RendererContext.hpp"
#include "VulkanContext.hpp"
#include <Core/Log.hpp>
#include <GLFW/glfw3.h>

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Type definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Static variables
***********************************************************************************************************************/
Engine::RendererContext* Engine::RendererContext::s_RendererContext = nullptr;

/***********************************************************************************************************************
RendererContext Class Implementation
***********************************************************************************************************************/

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
                VulkanContext::Create(VulkanSpec{rendererSpec, Window::GetRequiredExtensions().value}, result.value);
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