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
        DrawIndexed,
        DrawInstanced,
        BeginRenderPass,
        EndRenderPass,
        Submit,
        BeginFrame,
        Present,
        DrawQuadBatch,
        Count
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
    class Shader;

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

    class Shader;
    class VertexBuffer;
    template <typename T>
    class IndexBuffer;

    class RendererCommandDrawIndexed: public RendererCommand
    {
    public:
        RendererCommandDrawIndexed() = default;

        RendererCommandDrawIndexed(VertexBuffer* vb, IndexBuffer<uint16_t>* ib) : vb(vb), ib(ib) {}

        virtual RendererCommandType GetType() const override { return RendererCommandType::DrawIndexed; }

    public:
        VertexBuffer* vb{};
        IndexBuffer<uint16_t>* ib{};
    };

    class RendererCommandDrawInstanced: public RendererCommand
    {
    public:
        RendererCommandDrawInstanced() = default;

        RendererCommandDrawInstanced(VertexBuffer* vb, IndexBuffer<uint16_t>* ib, uint32_t count)
            : vb(vb), ib(ib), count(count)
        {}

        virtual RendererCommandType GetType() const override { return RendererCommandType::DrawInstanced; }

    public:
        VertexBuffer* vb{};
        IndexBuffer<uint16_t>* ib{};
        uint32_t count{};
    };

    template <typename T>
    class StorageBuffer;

    class RendererCommandDrawBatch: public RendererCommand
    {
    public:
        RendererCommandDrawBatch() = default;

        RendererCommandDrawBatch(uint8_t* data, StorageBuffer<uint8_t>* batchStorage, size_t count, size_t offset = 0)
            : data(data), batchStorage(batchStorage), count(count), offset(offset)
        {}

        virtual RendererCommandType GetType() const override { return RendererCommandType::DrawQuadBatch; }

    public:
        uint8_t* data{};
        StorageBuffer<uint8_t>* batchStorage{};
        size_t count{};
        size_t offset{};
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

    class RendererCommandBindShader: public RendererCommand
    {
    public:
        RendererCommandBindShader() = default;

        RendererCommandBindShader(Shader* shader, void* push_constants) : shader(shader), push_constants(push_constants)
        {}

        virtual RendererCommandType GetType() const override { return RendererCommandType::BindShader; }

    public:
        Shader* shader{};
        void* push_constants{};
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
        auto commands = std::array<RendererCommandType, 10>{
                RendererCommandType::BindTexture,     RendererCommandType::DrawTriangle,
                RendererCommandType::BeginRenderPass, RendererCommandType::EndRenderPass,
                RendererCommandType::Submit,          RendererCommandType::BeginFrame,
                RendererCommandType::Present,         RendererCommandType::BindTexture,
                RendererCommandType::DrawCircle,      RendererCommandType::DrawText};

        for (const auto& command: commands) { RegisterCommand<void>(command); }

        RegisterCommand<RendererCommandBindShader>(RendererCommandType::BindShader);
        RegisterCommand<RendererCommandClear>(RendererCommandType::Clear);
        RegisterCommand<RendererCommandDrawText>(RendererCommandType::DrawText);
        RegisterCommand<RendererCommandDrawIndexed>(RendererCommandType::DrawIndexed);
        RegisterCommand<RendererCommandDrawInstanced>(RendererCommandType::DrawInstanced);
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