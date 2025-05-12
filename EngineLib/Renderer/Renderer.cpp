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
#include "RendererAPI.hpp"
#include "Texture.hpp"
#include "Fonts.hpp"
#include <Core/Log.h>
#include <string_view>

namespace LunaraEngine
{

    RendererResultType Renderer::Init(std::string_view window_name, uint32_t width, uint32_t height)
    {
        RendererAPIConfig config;
        config.workingDirectory = std::filesystem::current_path();
        config.assetsDirectory = config.workingDirectory / "Assets";
        config.shadersDirectory = config.assetsDirectory / "Shaders";
        config.windowName = window_name;
        config.initialWidth = width;
        config.initialHeight = height;

        LOG_DEBUG("Initializing renderer...\n");
        LOG_DEBUG("Working directory: %s\n", config.workingDirectory.c_str());
        LOG_DEBUG("Assets directory: %s\n", config.assetsDirectory.c_str());
        LOG_DEBUG("Shaders directory: %s\n", config.shadersDirectory.c_str());
        LOG_DEBUG("Window name: %s\n", config.windowName.data());
        LOG_DEBUG("Initial width: %d\n", config.initialWidth);
        LOG_DEBUG("Initial height: %d\n", config.initialHeight);

        RendererAPI::CreateRendererAPI();
        RendererAPI::GetInstance()->Init(config);
        return RendererResultType::Renderer_Result_Not_Done;
    }

    void Renderer::Destroy() { RendererAPI::GetInstance()->Destroy(); }

    void Renderer::Present() {}

    void Renderer::DrawQuad(const FRect& rect, const Color4& color)
    {
        RendererCommandDrawQuad quad;
        quad.x = rect.x;
        quad.y = rect.y;
        quad.width = rect.w;
        quad.height = rect.h;
        quad.r = (uint8_t) color.r;
        quad.g = (uint8_t) color.g;
        quad.b = (uint8_t) color.b;
        quad.a = (uint8_t) color.a;
        (void) quad;
        //RendererCmdDrawQuad(&quad);
    }

    void Renderer::DrawTexture(float x, float y, Texture* texture)
    {
        RendererCommandDrawTexture tex;
        tex.x = x;
        tex.y = y;
        tex.texture = texture;
        (void) tex;
        // RendererCmdDrawTexture(&tex);
    }

    void Renderer::DrawCircle(float x, float y, float radius, const Color4& color)
    {
        RendererCommandDrawCircle circle;
        circle.x = x;
        circle.y = y;
        circle.radius = radius;
        circle.r = (uint8_t) color.r;
        circle.g = (uint8_t) color.g;
        circle.b = (uint8_t) color.b;
        circle.a = (uint8_t) color.a;
        (void) circle;
        //  RendererCmdDrawCircle(&circle);
    }

    void Renderer::DrawText(std::string_view text, Font* font, float x, float y, const Color4& color,
                            RendererTextAlignAttribute align)
    {
        RendererCommandDrawText text_cmd;
        text_cmd.text = (char*) text.data();
        text_cmd.font = (void*) font;
        text_cmd.x = x;
        text_cmd.y = y;
        text_cmd.r = (uint8_t) color.r;
        text_cmd.g = (uint8_t) color.g;
        text_cmd.b = (uint8_t) color.b;
        text_cmd.a = (uint8_t) color.a;
        text_cmd.align = align;
        (void) text_cmd;
        // RendererCmdDrawText(&text_cmd);
    }

    void Renderer::Clear(const Color4& color)
    {
        RendererCommandClear clear;
        clear.r = (uint8_t) color.r;
        clear.g = (uint8_t) color.g;
        clear.b = (uint8_t) color.b;
        clear.a = (uint8_t) color.a;
        (void) clear;
        //RendererCmdClear(&clear);
    }

    void Renderer::BeginRenderPass() {}

    void Renderer::EndRenderPass() {}

    void Renderer::Flush() {}

    Window* Renderer::GetWindow() { return RendererAPI::GetInstance()->GetWindow(); }


}// namespace LunaraEngine