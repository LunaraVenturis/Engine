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
        virtual void* GetBuffer(size_t binding) = 0;

    public:
        static size_t GetInputResourceSize(const ShaderInputResource& resource);
        static size_t GetFormatSize(ShaderResourceFormatT format);
        static size_t GetTypeSize(ShaderResourceDataTypeT type);
        static std::pair<ShaderResourceFormatT, ShaderResourceDataTypeT>
        GetDefaultFormatType(ShaderResourceAttributeType type);
        static std::string GetShaderResourceFormat(ShaderResourceFormatT format, ShaderResourceDataTypeT type);
        static std::string GetShaderResourceType(ShaderResourceFormatT format, ShaderResourceDataTypeT type);
        static size_t GetResourceAttributeTypeSize(ShaderResourceAttributeType type);

    protected:
        ShaderInfo p_Info;
    };

    class ShaderResourceBuilder
    {
    public:
        ShaderResourceBuilder(std::string_view name, ShaderResourceType type, size_t length = 1);
        ~ShaderResourceBuilder() = default;

    public:
        ShaderResourceBuilder& SetName(std::string_view name);
        ShaderResourceBuilder& SetType(ShaderResourceType type);
        ShaderResourceBuilder& SetLength(size_t length);
        ShaderResourceBuilder& SetLayoutType(ShaderResourceMemoryLayout layoutType);
        ShaderResourceBuilder& SetStride(size_t stride);
        ShaderResourceBuilder& AddAttribute(std::string_view name, ShaderResourceAttributeType type);
        ShaderResourceBuilder&
        AddAttributes(std::vector<std::pair<std::string_view, ShaderResourceAttributeType>>&& attributes);
        ShaderResourceBuilder& SetBinding(uint32_t binding);
        ShaderResource Build();

    private:
        ShaderResource m_Resource;
    };

    class ShaderInfoBuilder
    {
    public:
        ShaderInfoBuilder(ShaderInfo& result);

    public:
        void SetName(std::filesystem::path name);
        void SetPath(std::filesystem::path path);
        void UseAsComputeShader();
        void AddVertexInputResource(std::vector<std::pair<std::string_view, ShaderResourceAttributeType>>&& resources);
        void AddVertexInputResource(const ShaderInputResource& resource);
        void SetNumInstances(uint32_t numInstances);
        void AddResource(ShaderResource resource);

    private:
        ShaderInfo& m_Info;
    };
}// namespace LunaraEngine
