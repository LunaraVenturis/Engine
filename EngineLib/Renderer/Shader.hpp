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
        static std::shared_ptr<Shader> Create(ShaderType type, std::filesystem::path assetsDirectory);
        static std::shared_ptr<Shader> Create(const ShaderInfo& info);

    public:
        void Init(const ShaderInfo& info);

        virtual void SetUniform(std::string_view name, const float& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::vec2& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::vec3& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::vec4& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::mat3& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::mat4& value) = 0;
        virtual void SetUniform(std::string_view name, const int& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::ivec2& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::ivec3& value) = 0;
        virtual void SetUniform(std::string_view name, const glm::ivec4& value) = 0;

    public:
        static size_t GetInputResourceSize(const ShaderInputResource& resource);
        static size_t GetFormatSize(ShaderResourceFormatT format);
        static size_t GetTypeSize(ShaderResourceDataTypeT type);
        static std::string GetShaderResourceFormat(ShaderResourceFormatT format, ShaderResourceDataTypeT type);
        static std::string GetShaderResourceType(ShaderResourceFormatT format, ShaderResourceDataTypeT type);

    protected:
        ShaderInfo p_Info;
    };


}// namespace LunaraEngine
