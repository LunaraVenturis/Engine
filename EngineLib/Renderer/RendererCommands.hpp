#pragma once

namespace LunaraEngine
{
    /***********************************************************************************************************************
    Type definitions
    ***********************************************************************************************************************/
    enum class RendererResultType : int
    {
        Renderer_Result_None = 0,
        Renderer_Result_Success,
        Renderer_Result_Error,
        Renderer_Result_Not_Done,
    };

    enum class RendererCommandType : int
    {
        RendererCommandType_None = 0,
        RendererCommandType_BindShader,
        RendererCommandType_BindTexture,
        RendererCommandType_Clear,
        RendererCommandType_DrawQuad,
        RendererCommandType_DrawTriangle,
        RendererCommandType_DrawTexture,
        RendererCommandType_DrawCircle,
        RendererCommandType_DrawText,
        RendererCommandType_BeginRenderPass,
        RendererCommandType_EndRenderPass,
        RendererCommandType_Submit,
        RendererCommandType_BeginFrame,
        RendererCommandType_Present
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

    struct Texture;

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

    struct RendererCommandBeginRenderPass {
    };

    struct RendererCommandEndRenderPass {
    };

    struct RendererCommandFlush {
    };

    using RendererCommand = void;
}// namespace LunaraEngine