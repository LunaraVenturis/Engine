#include "Shader.hpp"
#include <Renderer/Vulkan/VulkanRendererAPI.hpp>
#include <Renderer/Vulkan/Shader.hpp>

namespace LunaraEngine
{
    Shader::Shader(ShaderInfo info) { Init(info); }

    Shader::~Shader() noexcept(false)
    {
        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                auto shader = (VulkanShader*) m_Handle;
                delete shader;
                break;
            }
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }

    void Shader::Init(ShaderInfo info)
    {
        m_Info = info;
        auto api = (VulkanRendererAPI*) RendererAPI::GetInstance();
        auto apiInstance = api->GetData();

        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                if (apiInstance.expired()) { return; }
                auto shader = new VulkanShader();
                auto apiInstancePtr = apiInstance.lock();
                shader->Create(apiInstancePtr.get(), info);
                m_Handle = (Shader*) shader;
                break;
            }
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }

    void Shader::SetUniform(std::string_view name, const glm::vec3& value)
    {
        auto api = (VulkanRendererAPI*) RendererAPI::GetInstance();
        auto apiInstance = api->GetData();

        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                if (apiInstance.expired()) { return; }
                VulkanShader* shader = (VulkanShader*) m_Handle;
                shader->SetUniform(name, value);
                break;
            }
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }

    Shader* Shader::GetHandle() { return m_Handle; }

    size_t Shader::GetFormatSize(ShaderResourceFormatT format)
    {
        switch (format)
        {
            case ShaderResourceFormatT::R8:
                return 1;
            case ShaderResourceFormatT::R16:
            case ShaderResourceFormatT::R8G8:
                return 2;
            case ShaderResourceFormatT::R8G8B8:
                return 3;
            case ShaderResourceFormatT::R32:
            case ShaderResourceFormatT::R16G16:
            case ShaderResourceFormatT::R8G8B8A8:
                return 4;
            case ShaderResourceFormatT::R16G16B16:
                return 6;
            case ShaderResourceFormatT::R32G32:
            case ShaderResourceFormatT::R16G16B16A16:
                return 8;
            case ShaderResourceFormatT::R32G32B32:
                return 12;
            case ShaderResourceFormatT::R32G32B32A32:
                return 16;
            default:
                return 0;
        }
    }

    size_t Shader::GetTypeSize(ShaderResourceDataTypeT type)
    {
        switch (type)
        {
            case ShaderResourceDataTypeT::SFloat:
                return 4;
            case ShaderResourceDataTypeT::SInt:
            case ShaderResourceDataTypeT::SNorm:
                return 4;
            case ShaderResourceDataTypeT::UInt:
            case ShaderResourceDataTypeT::UNorm:
                return 4;
            default:
                return 0;
        }
    }

    size_t Shader::GetInputResourceSize(const ShaderInputResource& resource)
    {
        auto formatSize = GetFormatSize(resource.format);
        return formatSize;
    }

    std::string Shader::GetShaderResourceFormat(ShaderResourceFormatT format, ShaderResourceDataTypeT type)
    {
        switch (format)
        {
            case ShaderResourceFormatT::R8:
                if (type == ShaderResourceDataTypeT::SFloat) return "R8_SRGB";
                else
                    return "R8_UNORM";
            case ShaderResourceFormatT::R16:
                if (type == ShaderResourceDataTypeT::SFloat) return "R16_SFLOAT";
                else
                    return "R16_UNORM";
            case ShaderResourceFormatT::R32:
                if (type == ShaderResourceDataTypeT::SFloat) return "R32_SFLOAT";
                else
                    return "R32_UINT";
            case ShaderResourceFormatT::R8G8:
                if (type == ShaderResourceDataTypeT::SFloat) return "R8G8_SRGB";
                else
                    return "R8G8_UNORM";
            case ShaderResourceFormatT::R16G16:
                if (type == ShaderResourceDataTypeT::SFloat) return "R16G16_SFLOAT";
                else
                    return "R16G16_UNORM";
            case ShaderResourceFormatT::R32G32:
                if (type == ShaderResourceDataTypeT::SFloat) return "R32G32_SFLOAT";
                else
                    return "R32G32_UINT";
            case ShaderResourceFormatT::R8G8B8:
                if (type == ShaderResourceDataTypeT::SFloat) return "R8G8B8_SRGB";
                else
                    return "R8G8B8_UNORM";
            case ShaderResourceFormatT::R16G16B16:
                if (type == ShaderResourceDataTypeT::SFloat) return "R16G16B16_SFLOAT";
                else
                    return "R16G16B16_UNORM";
            case ShaderResourceFormatT::R32G32B32:
                if (type == ShaderResourceDataTypeT::SFloat) return "R32G32B32_SFLOAT";
                else
                    return "R32G32B32_UINT";
            case ShaderResourceFormatT::R8G8B8A8:
                if (type == ShaderResourceDataTypeT::SFloat) return "R8G8B8A8_SRGB";
                else
                    return "R8G8B8A8_UNORM";
            case ShaderResourceFormatT::R16G16B16A16:
                if (type == ShaderResourceDataTypeT::SFloat) return "R16G16B16A16_SFLOAT";
                else
                    return "R16G16B16A16_UNORM";
            case ShaderResourceFormatT::R32G32B32A32:
                if (type == ShaderResourceDataTypeT::SFloat) return "R32G32B32A32_SFLOAT";
                else
                    return "R32G32B32A32_UINT";
            default:
                return "";
        }
    }

    std::string Shader::GetShaderResourceType(ShaderResourceFormatT format, ShaderResourceDataTypeT type)
    {
        switch (format)
        {
            case ShaderResourceFormatT::R8:
                if (type == ShaderResourceDataTypeT::SFloat) return "float";
                else
                    return "unorm";
            case ShaderResourceFormatT::R16:
                if (type == ShaderResourceDataTypeT::SFloat) return "float";
                else
                    return "unorm";
            case ShaderResourceFormatT::R32:
                if (type == ShaderResourceDataTypeT::SFloat) return "float";
                else
                    return "uint";
            case ShaderResourceFormatT::R8G8:
                if (type == ShaderResourceDataTypeT::SFloat) return "vec2";
                else if (type == ShaderResourceDataTypeT::UInt)
                    return "uivec2";
                else
                    return "ivec2";
            case ShaderResourceFormatT::R16G16:
                if (type == ShaderResourceDataTypeT::SFloat) return "vec2";
                else if (type == ShaderResourceDataTypeT::UInt)
                    return "uivec2";
                else
                    return "ivec2";
            case ShaderResourceFormatT::R32G32:
                if (type == ShaderResourceDataTypeT::SFloat) return "vec2";
                else if (type == ShaderResourceDataTypeT::UInt)
                    return "uivec2";
                else
                    return "ivec2";
            case ShaderResourceFormatT::R8G8B8:
                if (type == ShaderResourceDataTypeT::SFloat) return "vec3";
                else if (type == ShaderResourceDataTypeT::UInt)
                    return "uivec3";
                else
                    return "ivec3";
            case ShaderResourceFormatT::R16G16B16:
                if (type == ShaderResourceDataTypeT::SFloat) return "vec3";
                else if (type == ShaderResourceDataTypeT::UInt)
                    return "uivec3";
                else
                    return "ivec3";
            case ShaderResourceFormatT::R32G32B32:
                if (type == ShaderResourceDataTypeT::SFloat) return "vec3";
                else if (type == ShaderResourceDataTypeT::UInt)
                    return "uivec3";
                else
                    return "ivec3";
            case ShaderResourceFormatT::R8G8B8A8:
                if (type == ShaderResourceDataTypeT::SFloat) return "vec4";
                else if (type == ShaderResourceDataTypeT::UInt)
                    return "uivec4";
                else
                    return "ivec4";
            case ShaderResourceFormatT::R16G16B16A16:
                if (type == ShaderResourceDataTypeT::SFloat) return "vec4";
                else if (type == ShaderResourceDataTypeT::UInt)
                    return "uivec4";
                else
                    return "ivec4";
            case ShaderResourceFormatT::R32G32B32A32:
                if (type == ShaderResourceDataTypeT::SFloat) return "vec4";
                else if (type == ShaderResourceDataTypeT::UInt)
                    return "uivec4";
                else
                    return "ivec4";
            default:
                return "";
        }
    }
}// namespace LunaraEngine