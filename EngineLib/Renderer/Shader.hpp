#pragma once
#include <Renderer/CommonTypes.hpp>
#include <glm/glm.hpp>

namespace LunaraEngine
{

    class Shader
    {
    public:
        Shader() = default;
        Shader(ShaderInfo info);
        ~Shader() noexcept(false);

    public:
        void Init(ShaderInfo info);
        Shader* GetHandle();

        void SetUniform(std::string_view name, const glm::vec3& value);

    public:
        static size_t GetInputResourceSize(const ShaderInputResource& resource);
        static size_t GetFormatSize(ShaderResourceFormatT format);
        static size_t GetTypeSize(ShaderResourceDataTypeT type);
        static std::string GetShaderResourceFormat(ShaderResourceFormatT format, ShaderResourceDataTypeT type);
        static std::string GetShaderResourceType(ShaderResourceFormatT format, ShaderResourceDataTypeT type);

    public:
        ShaderInfo m_Info;
        Shader* m_Handle;
        uint8_t* m_Data;
    };
}// namespace LunaraEngine