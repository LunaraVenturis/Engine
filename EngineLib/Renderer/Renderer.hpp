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
 * Copyright (c) 2025 Krusto
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
#include "Fonts.h"
#include "Math/Rect.h"
#include "Math/Color.h"
#include <string_view>
#include <SDL3/SDL_render.h>
#include "Core/STDTypes.h"
#include "vulkan/vulkan.h"
#include <SDL3/SDL_vulkan.h>
struct Texture;

namespace LunaraEngine
{
    /***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
  constexpr int RENDERER_MAX_COMMANDS = 1024;

    /***********************************************************************************************************************
    Type definitions
    ***********************************************************************************************************************/
    enum class RendererResultType : int
    {
        Renderer_Result_None = 0,
        Renderer_Result_Success,
        Renderer_Result_Error
    };

    enum class RendererCommandType : int
    {
        RendererCommandType_None = 0,
        RendererCommandType_BindShader,
        RendererCommandType_BindTexture,
        RendererCommandType_Clear,
        RendererCommandType_DrawQuad,
        RendererCommandType_DrawTexture,
        RendererCommandType_DrawCircle,
        RendererCommandType_DrawText
    };

    struct RendererCommandDrawQuad {
        float x;
        float y;
        float width;
        float height;
        float r;
        float g;
        float b;
        float a;
    };

     struct RendererCommandClear {
        float r;
        float g;
        float b;
        float a;
    };

    enum class RendererTextAlignAttribute : int
    {
        TextAlign_TopLeft = 0,
        TextAlign_TopCenter,
        TextAlign_TopRight,
        TextAlign_Left,
        TextAlign_Center,
        TextAlign_Right,
        TextAlign_BottomLeft,
        TextAlign_BottomCenter,
        TextAlign_BottomRight,
    };

     struct RendererCommandDrawText {
        char* text;
        void* font;
        float x;
        float y;
        float r;
        float g;
        float b;
        float a;
        RendererTextAlignAttribute align;
    };

    struct RendererCommandDrawTexture {
        float x;
        float y;
        Texture* texture;
    };

    struct RendererCommandDrawCircle {
        float x;
        float y;
        float radius;
        float r;
        float g;
        float b;
        float a;
    };

    struct RendererCommandNode {

        void* data;
        void* next;
        RendererCommandType type;
    };

    using RendererCommand = void;

    struct RendererCommandStack {
        RendererCommandNode* head;
        RendererCommandNode* current;
    };

     struct Window {
        const char* name;
        void* data;
    };

     struct RendererDataType {
        RendererCommandStack command_stack;
        Window* window;
        SDL_Renderer* renderer;
        SDL_Surface* surface;
        VkInstance instance;
        float width;
        float height;
    };

     //to be fixed govnokod
     class Rend
     {
     public:
         static Rend& Get()
         { 
             return s_instance;
         }

         Rend(const Rend& other) = delete;
         void operator=(const Rend&) = delete;
         static Window* GetWindow() 
         { 
             return s_instance.renderer->window;
         }
     private:
         Rend() = default;
         RendererDataType* renderer;
         static Rend s_instance;
         
     };

     Rend Rend::s_instance;
    class Renderer
    {
    public:
        static RendererResultType Init(std::string_view window_name, uint32_t width, uint32_t height);
        static void Destroy();
        static void Present();
        static void DrawQuad(const FRect& rect, const Color4& color);
        static void DrawTexture(float x, float y, Texture* texture);
        static void DrawCircle(float x, float y, float radius, const Color4& color);
        static void DrawText(std::string_view text, Font* font, float x, float y, const Color4& color,
                             RendererTextAlignAttribute align = RendererTextAlignAttribute::TextAlign_TopLeft);
        static void Clear(const Color4& color);

        static void BeginRenderPass();
        static void EndRenderPass();
        static void Flush();
        static void CreateInstance();
        static Window* GetWindow();
    };
}// namespace LunaraEngine