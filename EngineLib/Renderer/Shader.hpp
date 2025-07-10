#pragma once
#include <Renderer/CommonTypes.hpp>
#include <glm/glm.hpp>

namespace LunaraEngine
{

    class Shader
    {
    public:
        Shader() = default;

        virtual ~Shader() noexcept(false);

    public:
        static std::shared_ptr<Shader> Create(ShaderType type, std::filesystem::path assetsDirectory,
                                              ShaderTypeInfo* shaderTypeInfo = nullptr);
        static std::shared_ptr<Shader> Create(const ShaderInfo& info);

    public:
        void Init(const ShaderInfo& info);

        virtual void SetUniform(std::string_view name, const float& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::vec2& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::vec3& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::vec4& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::mat3& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::mat4& value) = 0;
        virtual void SetUniform(std::string_view name, const uint32_t& value) = 0;
        virtual void SetUniform(std::string_view name, const int& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::ivec2& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::ivec3& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::ivec4& value) = 0;
        virtual void* GetBuffer(ShaderBinding binding) = 0;
        virtual void* GetTexture(ShaderBinding binding) = 0;

    public:
        static size_t GetInputResourceSize(const ShaderInputResource& resource);
        static size_t GetFormatSize(BufferResourceFormatT format);
        static size_t GetTypeSize(BufferResourceDataTypeT type);
        static std::pair<BufferResourceFormatT, BufferResourceDataTypeT>
        GetDefaultFormatType(BufferResourceAttributeType type);
        static std::string GetBufferResourceFormat(BufferResourceFormatT format, BufferResourceDataTypeT type);
        static std::string GetBufferResourceType(BufferResourceFormatT format, BufferResourceDataTypeT type);
        static size_t GetResourceAttributeTypeSize(BufferResourceAttributeType type);

    protected:
        ShaderInfo p_Info;
    };

    class BufferResourceBuilder
    {
    public:
        BufferResourceBuilder(std::string_view name, BufferResourceType type, size_t length = 1);
        ~BufferResourceBuilder() = default;

    public:
        BufferResourceBuilder& SetName(std::string_view name);
        BufferResourceBuilder& SetType(BufferResourceType type);
        BufferResourceBuilder& SetLength(size_t length);
        BufferResourceBuilder& SetLayoutType(BufferResourceMemoryLayout layoutType);
        BufferResourceBuilder& SetStride(size_t stride);
        BufferResourceBuilder& AddAttribute(std::string_view name, BufferResourceAttributeType type);
        BufferResourceBuilder&
        AddAttributes(std::vector<std::pair<std::string_view, BufferResourceAttributeType>>&& attributes);
        BufferResourceBuilder& SetBinding(ShaderBinding binding);
        BufferResource Build();

    private:
        BufferResource m_Resource;
    };

    class TextureResourceBuilder
    {
    public:
        TextureResourceBuilder(std::string_view name, BufferResourceType type, TextureInfo info);
        ~TextureResourceBuilder() = default;

    public:
        TextureResourceBuilder& SetName(std::string_view name);
        TextureResourceBuilder& SetType(BufferResourceType type);
        TextureResourceBuilder& SetBinding(ShaderBinding binding);
        TextureResource Build();

    private:
        TextureResource m_Resource;
    };

    class ShaderInfoBuilder
    {
    public:
        ShaderInfoBuilder(std::filesystem::path name, std::filesystem::path path, size_t numInstances = 1,
                          bool isCompute = false);

    public:
        ShaderInfoBuilder& SetName(std::filesystem::path name);
        ShaderInfoBuilder& SetPath(std::filesystem::path path);
        ShaderInfoBuilder& UseAsComputeShader();
        ShaderInfoBuilder&
        AddVertexInputResource(std::vector<std::pair<std::string_view, BufferResourceAttributeType>>&& resources);
        ShaderInfoBuilder& AddVertexInputResource(const ShaderInputResource& resource);
        ShaderInfoBuilder& SetNumInstances(uint32_t numInstances);
        ShaderInfoBuilder& AddResource(BufferResource resource);
        ShaderInfoBuilder& AddResource(TextureResource resource);
        ShaderInfoBuilder& AddResources(std::vector<BufferResource>&& resources);
        ShaderInfoBuilder& AddResources(std::vector<TextureResource>&& resources);
        ShaderInfo Build();

    private:
        ShaderInfo m_Info;
    };
}// namespace LunaraEngine
