#include "Shader.hpp"
#include <Renderer/Vulkan/VulkanRendererAPI.hpp>
#include <Renderer/Vulkan/Shader.hpp>
#include <Core/Log.h>

namespace LunaraEngine
{
    namespace FlatInstancedShader
    {
        inline static std::shared_ptr<Shader> Create(std::filesystem::path assetsDirectory)
        {
            ShaderResources basicShaderResources;
            basicShaderResources.bufferResources.push_back(ShaderResource{
                    .type = ShaderResourceType::UniformBuffer,
                    .name = "UniformBuffer",
                    .size = sizeof(glm::vec3),
                    .layout = ShaderResourceLayout{.binding = 0, .layoutType = ShaderResourceMemoryLayout::STD140},
                    .attributes = {
                            ShaderResourceAttribute{.name = "offset", .type = ShaderResourceAttributeType::Vec3},
                    }});

            ShaderInfo basicShaderInfo;
            basicShaderInfo.isComputeShader = false;
            basicShaderInfo.name = L"FlatInstanced";
            basicShaderInfo.path = assetsDirectory / "Shaders/output";
            basicShaderInfo.resources = basicShaderResources;

            basicShaderInfo.resources.inputResources.push_back(
                    ShaderInputResource{.name = "Position",
                                        .binding = 0,
                                        .location = 0,
                                        .format = ShaderResourceFormatT::R32G32,
                                        .type = ShaderResourceDataTypeT::SFloat,
                                        .offset = 0});
            basicShaderInfo.resources.inputResources.push_back(
                    ShaderInputResource{.name = "Color",
                                        .binding = 0,
                                        .location = 1,
                                        .format = ShaderResourceFormatT::R32G32B32,
                                        .type = ShaderResourceDataTypeT::SFloat,
                                        .offset = 8});
            return Shader::Create(basicShaderInfo);
        }
    }// namespace FlatInstancedShader

    namespace FlatShader
    {
        inline static std::shared_ptr<Shader> Create(std::filesystem::path assetsDirectory)
        {
            ShaderResources basicShaderResources;
            basicShaderResources.bufferResources.push_back(ShaderResource{
                    .type = ShaderResourceType::UniformBuffer,
                    .name = "UniformBuffer",
                    .size = sizeof(glm::vec2),
                    .layout = ShaderResourceLayout{.binding = 0, .layoutType = ShaderResourceMemoryLayout::STD140},
                    .attributes = {
                            ShaderResourceAttribute{.name = "offset", .type = ShaderResourceAttributeType::Vec2},
                    }});

            ShaderInfo basicShaderInfo;
            basicShaderInfo.isComputeShader = false;
            basicShaderInfo.name = L"FlatQuad";
            basicShaderInfo.path = assetsDirectory / "Shaders/output";
            basicShaderInfo.resources = basicShaderResources;

            basicShaderInfo.resources.inputResources.push_back(
                    ShaderInputResource{.name = "Position",
                                        .binding = 0,
                                        .location = 0,
                                        .format = ShaderResourceFormatT::R32G32,
                                        .type = ShaderResourceDataTypeT::SFloat,
                                        .offset = 0});
            basicShaderInfo.resources.inputResources.push_back(
                    ShaderInputResource{.name = "Color",
                                        .binding = 0,
                                        .location = 1,
                                        .format = ShaderResourceFormatT::R32G32B32,
                                        .type = ShaderResourceDataTypeT::SFloat,
                                        .offset = 8});
            return Shader::Create(basicShaderInfo);
        }
    }// namespace FlatShader

    std::shared_ptr<Shader> Shader::Create(ShaderType type, std::filesystem::path assetsDirectory)
    {
        switch (type)
        {
            case ShaderType::FlatInstanced:
                return FlatInstancedShader::Create(assetsDirectory);
            case ShaderType::FlatQuad:
                return FlatShader::Create(assetsDirectory);
            default:
                throw std::runtime_error("Unknown shader type");
        }
    }

    std::shared_ptr<Shader> Shader::Create(const ShaderInfo& info)
    {
        auto api = (VulkanRendererAPI*) RendererAPI::GetInstance();
        auto apiInstance = api->GetData();

        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                if (apiInstance.expired()) { throw std::runtime_error("VulkanRendererAPI instance is expired"); }
                return std::make_shared<VulkanShader>(apiInstance.lock().get(), info);
            }
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }

    Shader::~Shader() noexcept(false) { LOG_DEBUG("Shader destroyed"); }

    void Shader::Init(const ShaderInfo& info)
    {
        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                auto api = (VulkanRendererAPI*) RendererAPI::GetInstance();
                auto apiInstance = api->GetData();
                if (apiInstance.expired()) { return; }
                ((VulkanShader*) this)->Init(apiInstance.lock().get(), info);
                break;
            }
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }

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