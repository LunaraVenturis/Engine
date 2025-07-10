/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com ) 
 * @coauthor Neyko Naydenov ( neyko641@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section LICENSE
 * MIT License
 * 
 * Copyright (c) 2025 Krusto, Neyko
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
 * Renderer definitions
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "Renderer.hpp"
#include "BatchRenderer.hpp"
#include "RendererAPI.hpp"
#include "Buffer/Texture.hpp"
#include "Fonts.hpp"
#include <Renderer/RendererCommands.hpp>
#include <Core/Log.h>
#include <string_view>

namespace LunaraEngine
{
    const char* RendererCommand::GetName() { return s_RegisteredCommandNames.at(GetType()); }

    RendererResultType Renderer::Init(std::string_view window_name, uint32_t width, uint32_t height)
    {
        RendererAPIConfig config;
        config.workingDirectory = std::filesystem::current_path();
        config.assetsDirectory = config.workingDirectory / "Assets";
        config.shadersDirectory = config.assetsDirectory / "Shaders";
        config.windowName = window_name;
        config.initialWidth = width;
        config.initialHeight = height;

        LOG_DEBUG("Initializing renderer...");
        LOG_DEBUG("Working directory: %s", config.workingDirectory.c_str());
        LOG_DEBUG("Assets directory: %s", config.assetsDirectory.c_str());
        LOG_DEBUG("Shaders directory: %s", config.shadersDirectory.c_str());
        LOG_DEBUG("Window name: %s", config.windowName.data());
        LOG_DEBUG("Initial width: %d", config.initialWidth);
        LOG_DEBUG("Initial height: %d", config.initialHeight);

        s_Instance = new Renderer();

        RendererAPI::CreateRendererAPI();
        RendererAPI::GetInstance()->Init(config);

        RendererCommand::RegisterCommands();

        LOG_DEBUG("Renderer initialized");

        return RendererResultType::Renderer_Result_Not_Done;
    }

    void Renderer::Destroy()
    {
        RendererAPI::GetInstance()->Destroy();
        RendererAPI::DestroyRendererAPI();

        delete s_Instance;
        s_Instance = nullptr;
    }

    void Renderer::BeginFrame() { PushCommand(RendererCommandType::BeginFrame); }

    void Renderer::Present() { PushCommand(RendererCommandType::Present); }

    void Renderer::DrawTriangle() { PushCommand(RendererCommandType::DrawTriangle); }

    void Renderer::BindShader(Shader* shader, void* push_constants)
    {
        PushCommand(new RendererCommandBindShader(shader, push_constants));
    }

    void Renderer::DrawQuad(const FRect& rect, const Color4& color)
    {
        PushCommand(new RendererCommandDrawQuad(rect.x, rect.y, rect.w, rect.h, color.r, color.g, color.b, color.a));
    }

    void Renderer::DrawTexture(float x, float y, Texture* texture)
    {
        PushCommand(new RendererCommandDrawTexture(x, y, texture));
    }

    void Renderer::DrawCircle(float x, float y, float radius, const Color4& color)
    {
        PushCommand(new RendererCommandDrawCircle(x, y, radius, color.r, color.g, color.b, color.a));
    }

    void Renderer::DrawText(std::string_view text, Font* font, float x, float y, const Color4& color,
                            RendererTextAlignAttribute align)
    {
        PushCommand(new RendererCommandDrawText(text.data(), font, x, y, color.r, color.g, color.b, color.a, align));
    }

    void Renderer::Clear(const Color4& color)
    {
        PushCommand(new RendererCommandClear(color.r, color.g, color.b, color.a));
    }

    void Renderer::BeginRenderPass() { PushCommand(RendererCommandType::BeginRenderPass); }

    void Renderer::EndRenderPass() { PushCommand(RendererCommandType::EndRenderPass); }

    void Renderer::DrawQuadBatch()
    {
        PushCommand(BatchRenderer::CreateDrawCommand());
        BatchRenderer::Flush();
    }

    void Renderer::Flush()
    {
        for (const auto& cmd: Renderer::GetInstance()->m_CommandStack)
        {
            if (!std::holds_alternative<RendererCommand*>(cmd))
            {
                RendererAPI::GetInstance()->HandleCommand(nullptr, std::get<RendererCommandType>(cmd));
                continue;
            }
            else
            {
                RendererAPI::GetInstance()->HandleCommand(
                        std::get<RendererCommand*>(cmd), std::get<RendererCommand*>(cmd)->GetType());
                RendererCommand::FreeCommand(std::get<RendererCommand*>(cmd));
            }
        }

        Renderer::GetInstance()->m_CommandStack.clear();
    }

    size_t Renderer::GetWidth() { return size_t(); }

    size_t Renderer::GetHeight() { return size_t(); }

    Window* Renderer::GetWindow() { return RendererAPI::GetInstance()->GetWindow(); }


}// namespace LunaraEngine
