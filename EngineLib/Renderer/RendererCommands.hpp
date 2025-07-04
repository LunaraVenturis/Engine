#pragma once
#include <map>
#include <functional>
#include <string_view>
#include <span>
#include <memory>
#include <utility>
#include <Core/Log.h>
#include <Renderer/Shader.hpp>

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

        template <typename T,
                  std::enable_if_t<std::is_base_of_v<RendererCommand, T> && !std::is_same_v<T, void>, int> = 0>
        constexpr static void RegisterCommand(RendererCommandType type, std::string_view name);

        template <typename T, std::enable_if_t<std::is_same_v<void, T>, int> = 0>
        constexpr static void RegisterCommand(RendererCommandType type, std::string_view name);

        const char* GetName();

    private:
        inline static std::map<RendererCommandType, std::function<void(RendererCommand*)>> s_RegisteredCommands;
        inline static std::map<RendererCommandType, const char*> s_RegisteredCommandNames;
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
    enum class ShaderBinding : size_t;

    template <typename T = uint8_t>
    class BaseBufferUploadList
    {

    public:
        template <typename Ty = uint8_t>
        using BufferView = std::span<Ty>;

        template <typename Ty = uint8_t>
        using BufferUpload = std::tuple<StorageBuffer<Ty>*, BufferView<Ty>>;


        BaseBufferUploadList() = default;
        ~BaseBufferUploadList() = default;

        explicit BaseBufferUploadList(std::vector<BufferUpload<T>>&& list) { this->list = std::move(list); }

    public:
        template <std::ranges::range Container, typename V = void>
        BaseBufferUploadList& Add(V* dstBuffer, const Container& srcBuffer)
        {
            using U = std::ranges::range_value_t<Container>;
            static_assert(std::is_trivially_copyable_v<U>);

            list.emplace_back((StorageBuffer<T>*) dstBuffer, BufferView<T>((T*) srcBuffer.data(), srcBuffer.size()));
            return *this;
        }

        template <typename U, typename V = void>

        requires std::is_trivially_copyable_v<U> BaseBufferUploadList& Add(V* dstBuffer, U* srcBuffer, size_t length)
        {
            list.emplace_back((StorageBuffer<T>*) dstBuffer, BufferView<T>((T*) srcBuffer, length));
            return *this;
        }

    public:
        auto begin() { return list.begin(); }

        auto begin() const { return list.begin(); }

        auto end() { return list.end(); }

        auto end() const { return list.end(); }

    protected:
        std::vector<BufferUpload<T>> list;
    };

    template <typename T = uint8_t>
    class BaseBufferUploadListBuilder
    {
    public:
        BaseBufferUploadListBuilder(std::weak_ptr<Shader> shader) : m_Shader(shader) {}

        ~BaseBufferUploadListBuilder() = default;

    public:
        BaseBufferUploadList<T>& Get() { return m_List; }

    public:
        template <std::ranges::range Container>

        requires(std::is_trivially_copyable_v<std::ranges::range_value_t<Container>>)
                BaseBufferUploadListBuilder& Add(const Container& srcBuffer)
        {
            if (m_Shader.expired())
            {
                LOG_ERROR("Shader is expired");
                return *this;
            }

            auto shader = m_Shader.lock();

            auto* buffer = shader->GetBuffer(m_LastBinding);
            m_List.template Add<Container>(buffer, srcBuffer);
            m_LastBinding = (ShaderBinding) ((size_t) m_LastBinding + 1);
            return *this;
        }

        template <std::ranges::range... Containers>

        requires((std::is_trivially_copyable_v<std::ranges::range_value_t<Containers>> &&
                  ...)) BaseBufferUploadListBuilder& Add(const Containers&... srcBuffers)
        {
            (Add(srcBuffers), ...);
            return *this;
        }

        template <typename U>

        requires(std::is_trivially_copyable_v<U>)
                [[nodiscard]] BaseBufferUploadListBuilder& Add(U* srcBuffer, size_t length)
        {
            Add(std::span{std::in_place_t{}, srcBuffer, length});
            return *this;
        }

    private:
        ShaderBinding m_LastBinding{ShaderBinding::_1};
        std::weak_ptr<Shader> m_Shader;
        BaseBufferUploadList<T> m_List;
    };

    using BufferUploadList = BaseBufferUploadList<>;
    using BufferUploadListBuilder = BaseBufferUploadListBuilder<>;

    class RendererCommandDrawBatch: public RendererCommand
    {
    public:
        RendererCommandDrawBatch() = default;

        RendererCommandDrawBatch(BaseBufferUploadList<>&& uploadList, size_t count, size_t offset = 0)
            : uploadList(std::move(uploadList)), count(count), offset(offset)
        {}

        RendererCommandDrawBatch(const BaseBufferUploadList<>& uploadList, size_t count, size_t offset = 0)
            : uploadList(uploadList), count(count), offset(offset)
        {}

        virtual RendererCommandType GetType() const override { return RendererCommandType::DrawQuadBatch; }

    public:
        BaseBufferUploadList<> uploadList;
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
        auto commands = std::array<std::tuple<RendererCommandType, std::string_view>, 9>{
                std::tuple<RendererCommandType, std::string_view>{RendererCommandType::BindTexture,
                                                                  "RendererCommand::BindTexture"},
                {RendererCommandType::DrawTriangle, "RendererCommand::DrawTriangle"},
                {RendererCommandType::BeginRenderPass, "RendererCommand::BeginRenderPass"},
                {RendererCommandType::EndRenderPass, "RendererCommand::EndRenderPass"},
                {RendererCommandType::Submit, "RendererCommand::Submit"},
                {RendererCommandType::BeginFrame, "RendererCommand::BeginFrame"},
                {RendererCommandType::Present, "RendererCommand::Present"},
                {RendererCommandType::DrawCircle, "RendererCommand::DrawCircle"},
                {RendererCommandType::DrawText, "RendererCommand::DrawText"}};

        for (const auto& [command, commandName]: commands) { RegisterCommand<void>(command, commandName); }

        RegisterCommand<RendererCommandBindShader>(RendererCommandType::BindShader, "RendererCommand::BindShader");
        RegisterCommand<RendererCommandClear>(RendererCommandType::Clear, "RendererCommand::Clear");
        RegisterCommand<RendererCommandDrawText>(RendererCommandType::DrawText, "RendererCommand::DrawText");
        RegisterCommand<RendererCommandDrawIndexed>(RendererCommandType::DrawIndexed, "RendererCommand::DrawIndexed");
        RegisterCommand<RendererCommandDrawInstanced>(RendererCommandType::DrawInstanced,
                                                      "RendererCommand::DrawInstanced");
        RegisterCommand<RendererCommandDrawInstanced>(RendererCommandType::DrawQuadBatch,
                                                      "RendererCommand::DrawQuadBatch");
    }

    template <typename T, std::enable_if_t<std::is_base_of_v<RendererCommand, T> && !std::is_same_v<T, void>, int>>
    constexpr void RendererCommand::RegisterCommand(RendererCommandType type, std::string_view name)
    {
        LOG_INFO("Registering command: %s", name.data());
        RendererCommand::s_RegisteredCommands.emplace(type, [](RendererCommand* command) { delete (T*) (command); });
        RendererCommand::s_RegisteredCommandNames.emplace(type, name.data());
    }

    template <typename T, std::enable_if_t<std::is_same_v<void, T>, int>>
    constexpr void RendererCommand::RegisterCommand(RendererCommandType type, std::string_view name)
    {
        LOG_INFO("Registering command: %s", name.data());
        RendererCommand::s_RegisteredCommands.emplace(type, nullptr);
        RendererCommand::s_RegisteredCommandNames.emplace(type, name.data());
    }

}// namespace LunaraEngine