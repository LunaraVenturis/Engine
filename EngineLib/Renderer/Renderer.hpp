/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com ) 
 * @coauthor Neyko Naydenov (neyko641@gmail.com)
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
 * Renderer declarations
 */

#pragma once

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "Core/STDTypes.h"
#include "Window.hpp"
#include "Math/Rect.h"
#include "Math/Color.h"
#include "RendererCommands.hpp"
#include "Fonts.hpp"
#include "Texture.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "Shader.hpp"

#include <SDL3/SDL_render.h>

#include <string_view>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <Core/Log.h>

namespace LunaraEngine
{

    class Renderer
    {
    public:
        Renderer() = default;
        ~Renderer() = default;

    public:
        static RendererResultType Init(std::string_view window_name, uint32_t width, uint32_t height);
        static void Destroy();
        static void BeginFrame();
        static void Present();
        static void BindShader(Shader* shader);
        static void DrawQuad(const FRect& rect, const Color4& color);
        static void DrawTexture(float x, float y, Texture* texture);
        static void DrawTriangle();
        static void DrawCircle(float x, float y, float radius, const Color4& color);
        static void DrawText(std::string_view text, Font* font, float x, float y, const Color4& color,
                             RendererTextAlignAttribute align = RendererTextAlignAttribute::TextAlign_TopLeft);
        template <typename T>
        static void DrawIndexed(VertexBuffer* vb, IndexBuffer<T>* ib);
        template <typename T>
        static void DrawInstanced(VertexBuffer* vb, IndexBuffer<T>* ib, uint32_t count);
        static void Clear(const Color4& color);
        static void BeginRenderPass();
        static void EndRenderPass();
        static void Flush();

    public:
        static Window* GetWindow();

        inline static Renderer* GetInstance() { return s_Instance; }

    private:
        inline static void PushCommand(RendererCommand* command)
        {
            GetInstance()->m_CommandStack.push_back(std::variant<RendererCommandType, RendererCommand*>(command));
        }

        inline static void PushCommand(RendererCommandType type)
        {
            GetInstance()->m_CommandStack.push_back(std::variant<RendererCommandType, RendererCommand*>(type));
        }

    private:
        inline static Renderer* s_Instance{};

    private:
        std::vector<std::variant<RendererCommandType, RendererCommand*>> m_CommandStack;
    };
}// namespace LunaraEngine

#include <Renderer/Renderer.inl>