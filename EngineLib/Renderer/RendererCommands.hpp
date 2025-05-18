#pragma once
#include <map>
#include <functional>

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
        None = 0,
        BindShader,
        BindTexture,
        Clear,
        DrawQuad,
        DrawTriangle,
        DrawTexture,
        DrawCircle,
        DrawText,
        BeginRenderPass,
        EndRenderPass,
        Submit,
        BeginFrame,
        Present
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

    struct Texture;

    class RendererCommand
    {
    public:
        virtual ~RendererCommand() = default;

        virtual RendererCommandType GetType() const = 0;


    public:
        static void FreeCommand(RendererCommand* command)
        {
            const auto& freecmd = RendererCommand::s_RegisteredCommands[command->GetType()];
            if (freecmd != nullptr) { freecmd(command); }
        }

        constexpr static void RegisterCommands();

        template <typename T, std::enable_if_t<std::is_base_of<RendererCommand, T>::value>>
        constexpr static void RegisterCommand(RendererCommandType type);

        template <typename T>
        constexpr static void RegisterCommand(RendererCommandType type);

    private:
        inline static std::map<RendererCommandType, std::function<void(RendererCommand*)>> s_RegisteredCommands;
    };

    class RendererCommandDrawQuad: public RendererCommand
    {
    public:
        RendererCommandDrawQuad() = default;

        RendererCommandDrawQuad(float x, float y, float width, float height, float r, float g, float b, float a)
            : x(x), y(y), width(width), height(height), r(r), g(g), b(b), a(a)
        {}

        virtual RendererCommandType GetType() const override { return RendererCommandType::DrawQuad; }

    public:
        float x{};
        float y{};
        float width{};
        float height{};
        float r{};
        float g{};
        float b{};
        float a{};
    };

    class RendererCommandDrawCircle: public RendererCommand
    {
    public:
        RendererCommandDrawCircle() = default;

        RendererCommandDrawCircle(float x, float y, float radius, float r, float g, float b, float a)
            : x(x), y(y), radius(radius), r(r), g(g), b(b), a(a)
        {}

        virtual RendererCommandType GetType() const override { return RendererCommandType::DrawCircle; }

    public:
        float x{};
        float y{};
        float radius{};
        float r{};
        float g{};
        float b{};
        float a{};
    };

    class RendererCommandDrawText: public RendererCommand
    {
    public:
        RendererCommandDrawText() = default;

        RendererCommandDrawText(const char* text, void* font, float x, float y, float r, float g, float b, float a,
                                RendererTextAlignAttribute align)
            : text(text), font(font), x(x), y(y), r(r), g(g), b(b), a(a), align(align)
        {}

        virtual RendererCommandType GetType() const override { return RendererCommandType::DrawText; }

    public:
        const char* text{};
        void* font{};
        float x{};
        float y{};
        float r{};
        float g{};
        float b{};
        float a{};
        RendererTextAlignAttribute align{};
    };

    class RendererCommandDrawTexture: public RendererCommand
    {
    public:
        RendererCommandDrawTexture() = default;

        RendererCommandDrawTexture(float x, float y, Texture* texture) : x(x), y(y), texture(texture) {}

        virtual RendererCommandType GetType() const override { return RendererCommandType::DrawTexture; }

    public:
        float x{};
        float y{};
        Texture* texture{};
    };

    class RendererCommandClear: public RendererCommand
    {
    public:
        RendererCommandClear() = default;

        RendererCommandClear(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

        virtual RendererCommandType GetType() const override { return RendererCommandType::Clear; }

    public:
        float r{};
        float g{};
        float b{};
        float a{};
    };

    constexpr void RendererCommand::RegisterCommands()
    {
        auto commands = std::array<RendererCommandType, 8>{
                RendererCommandType::BindShader,    RendererCommandType::BindTexture,
                RendererCommandType::DrawTriangle,  RendererCommandType::BeginRenderPass,
                RendererCommandType::EndRenderPass, RendererCommandType::Submit,
                RendererCommandType::BeginFrame,    RendererCommandType::Present};

        for (const auto& command: commands) { RegisterCommand<void>(command); }

        RegisterCommand<RendererCommandClear>(RendererCommandType::Clear);
        RegisterCommand<RendererCommandDrawQuad>(RendererCommandType::DrawQuad);
        RegisterCommand<RendererCommandDrawTexture>(RendererCommandType::DrawTexture);
        RegisterCommand<RendererCommandDrawCircle>(RendererCommandType::DrawCircle);
        RegisterCommand<RendererCommandDrawText>(RendererCommandType::DrawText);
    }

    template <typename T, std::enable_if_t<std::is_base_of<RendererCommand, T>::value>>
    constexpr void RendererCommand::RegisterCommand(RendererCommandType type)
    {
        RendererCommand::s_RegisteredCommands.emplace(type, [](RendererCommand* command) { delete (T*) (command); });
    }

    template <typename T>
    constexpr void RendererCommand::RegisterCommand(RendererCommandType type)
    {
        RendererCommand::s_RegisteredCommands.emplace(type, nullptr);
    }

}// namespace LunaraEngine