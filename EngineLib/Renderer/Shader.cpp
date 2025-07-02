#include "Shader.hpp"
#include <Renderer/Vulkan/VulkanRendererAPI.hpp>
#include <Renderer/Vulkan/Shader.hpp>
#include <Core/Log.h>

namespace LunaraEngine
{
    namespace FlatInstancedShader
    {
        inline static std::shared_ptr<Shader> Create(std::filesystem::path assetsDirectory,
                                                     FlatInstancedShaderInfo* info)
        {
            ShaderResources basicShaderResources;
            basicShaderResources.bufferResources.push_back(ShaderResource{
                    .type = ShaderResourceType::UniformBuffer,
                    .name = "UniformBuffer",
                    .length = 1,
                    .stride = sizeof(glm::mat4) * 3 + sizeof(float),
                    .layout = ShaderResourceLayout{.binding = 0, .layoutType = ShaderResourceMemoryLayout::STD140},
                    .attributes = {
                            ShaderResourceAttribute{.name = "model", .type = ShaderResourceAttributeType::Mat4},
                            ShaderResourceAttribute{.name = "view", .type = ShaderResourceAttributeType::Mat4},
                            ShaderResourceAttribute{.name = "projection", .type = ShaderResourceAttributeType::Mat4},
                            ShaderResourceAttribute{.name = "zoom", .type = ShaderResourceAttributeType::Float},
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
            basicShaderInfo.numInstances = (uint32_t) info->numInstances;


            // ShaderInfo shaderInfo;
            // ShaderInfoBuilder shaderBuilder(shaderInfo);
            // shaderBuilder.SetName(L"FlatInstanced");
            // shaderBuilder.SetPath(assetsDirectory / "Shaders/output");
            // shaderBuilder.SetNumInstances(info->numInstances);
            // shaderBuilder.AddVertexInputResource({
            //         {"Position", ShaderResourceAttributeType::Vec2},
            //         {"Color", ShaderResourceAttributeType::Vec3},
            // });

            // shaderBuilder.AddResource(ShaderResourceBuilder()
            //                                   .SetName("UniformBuffer")
            //                                   .SetType(ShaderResourceType::UniformBuffer)
            //                                   .AddAttribute("model", ShaderResourceAttributeType::Mat4)
            //                                   .AddAttribute("view", ShaderResourceAttributeType::Mat4)
            //                                   .AddAttribute("projection", ShaderResourceAttributeType::Mat4)
            //                                   .AddAttribute("zoom", ShaderResourceAttributeType::Float)
            //                                   .Build());

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
                    .length = 1,
                    .stride = sizeof(glm::mat4) * 3 + sizeof(float),
                    .layout = ShaderResourceLayout{.binding = 0, .layoutType = ShaderResourceMemoryLayout::STD140},
                    .attributes = {
                            ShaderResourceAttribute{.name = "model", .type = ShaderResourceAttributeType::Mat4},
                            ShaderResourceAttribute{.name = "view", .type = ShaderResourceAttributeType::Mat4},
                            ShaderResourceAttribute{.name = "projection", .type = ShaderResourceAttributeType::Mat4},
                            ShaderResourceAttribute{.name = "zoom", .type = ShaderResourceAttributeType::Float},
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

    std::shared_ptr<Shader> Shader::Create(ShaderType type, std::filesystem::path assetsDirectory,
                                           ShaderTypeInfo* shaderTypeInfo)
    {
        switch (type)
        {
            case ShaderType::FlatInstanced:
                return FlatInstancedShader::Create(assetsDirectory, (FlatInstancedShaderInfo*) shaderTypeInfo);
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


        LOG_DEBUG("Creating shader %ls %s", info.name.c_str(), info.path.c_str());

        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                if (apiInstance.expired()) { throw std::runtime_error("VulkanRendererAPI instance is expired"); }
                auto apiInstancePtr = apiInstance.lock();
                auto shader = std::make_shared<VulkanShader>(apiInstancePtr.get(), info);
                return std::static_pointer_cast<Shader>(shader);
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

    std::pair<ShaderResourceFormatT, ShaderResourceDataTypeT>
    Shader::GetDefaultFormatType(ShaderResourceAttributeType type)
    {
        switch (type)
        {
            case ShaderResourceAttributeType::Float:
                return std::make_pair(ShaderResourceFormatT::R32, ShaderResourceDataTypeT::SFloat);
            case ShaderResourceAttributeType::Int:
                return std::make_pair(ShaderResourceFormatT::R32, ShaderResourceDataTypeT::SInt);
            case ShaderResourceAttributeType::UInt:
                return std::make_pair(ShaderResourceFormatT::R32, ShaderResourceDataTypeT::UInt);
            case ShaderResourceAttributeType::Vec2:
                return std::make_pair(ShaderResourceFormatT::R32G32, ShaderResourceDataTypeT::SFloat);
            case ShaderResourceAttributeType::Vec3:
                return std::make_pair(ShaderResourceFormatT::R32G32B32, ShaderResourceDataTypeT::SFloat);
            case ShaderResourceAttributeType::Vec4:
                return std::make_pair(ShaderResourceFormatT::R32G32B32A32, ShaderResourceDataTypeT::SFloat);
            case ShaderResourceAttributeType::IVec2:
                return std::make_pair(ShaderResourceFormatT::R32G32, ShaderResourceDataTypeT::SInt);
            case ShaderResourceAttributeType::IVec3:
                return std::make_pair(ShaderResourceFormatT::R32G32B32, ShaderResourceDataTypeT::SInt);
            case ShaderResourceAttributeType::Mat2:
                return std::make_pair(ShaderResourceFormatT::R32G32B32A32, ShaderResourceDataTypeT::SFloat);
            case ShaderResourceAttributeType::Mat3:
                return std::make_pair(ShaderResourceFormatT::R32G32B32A32, ShaderResourceDataTypeT::SFloat);
            case ShaderResourceAttributeType::Mat4:
                return std::make_pair(ShaderResourceFormatT::R32G32B32A32, ShaderResourceDataTypeT::SFloat);
            default:
                return std::make_pair(ShaderResourceFormatT::R32, ShaderResourceDataTypeT::SFloat);
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

    size_t Shader::GetResourceAttributeTypeSize(ShaderResourceAttributeType type)
    {
        switch (type)
        {
            case ShaderResourceAttributeType::Float:
                return sizeof(float);
            case ShaderResourceAttributeType::Vec2:
                return sizeof(float) * 2;
            case ShaderResourceAttributeType::Vec3:
                return sizeof(float) * 3;
            case ShaderResourceAttributeType::Vec4:
                return sizeof(float) * 4;
            case ShaderResourceAttributeType::Mat2:
                return sizeof(float) * 4;
            case ShaderResourceAttributeType::Mat3:
                return sizeof(float) * 9;
            case ShaderResourceAttributeType::Mat4:
                return sizeof(float) * 16;
            case ShaderResourceAttributeType::UInt:
                return sizeof(uint32_t);
            case ShaderResourceAttributeType::Int:
                return sizeof(int32_t);
            case ShaderResourceAttributeType::IVec2:
                return sizeof(int32_t) * 2;
            case ShaderResourceAttributeType::IVec3:
                return sizeof(int32_t) * 3;
            case ShaderResourceAttributeType::IVec4:
                return sizeof(int32_t) * 4;
            default:
                return 0;
        }
    }

    ShaderInfoBuilder::ShaderInfoBuilder(ShaderInfo& result) : m_Info(result) {}

    void ShaderInfoBuilder::SetName(std::filesystem::path name) { m_Info.name = name.wstring(); }

    void ShaderInfoBuilder::SetPath(std::filesystem::path path) { m_Info.path = path; }

    void ShaderInfoBuilder::UseAsComputeShader() { m_Info.isComputeShader = true; }

    void ShaderInfoBuilder::AddVertexInputResource(
            std::vector<std::pair<std::string_view, ShaderResourceAttributeType>>&& resources)
    {
        const size_t binding{};
        uint32_t location{};
        uint32_t offset{};
        for (auto& [name, type]: resources)
        {
            auto [shaderResourceFormat, shaderResourceType] = Shader::GetDefaultFormatType(type);

            m_Info.resources.inputResources.push_back(ShaderInputResource{.name = name,
                                                                          .binding = binding,
                                                                          .location = location,
                                                                          .format = shaderResourceFormat,
                                                                          .type = shaderResourceType,
                                                                          .offset = offset});
            offset += Shader::GetFormatSize(shaderResourceFormat);
            ++location;
        }
    }

    void ShaderInfoBuilder::SetNumInstances(uint32_t numInstances) { m_Info.numInstances = numInstances; }

    void ShaderInfoBuilder::AddVertexInputResource(const ShaderInputResource& resource)
    {
        m_Info.resources.inputResources.push_back(resource);
    }

    void ShaderInfoBuilder::AddResource(ShaderResource resource)
    {
        if (resource.layout.binding == SHADER_ALL_BINDINGS)
        {
            resource.layout.binding = static_cast<uint32_t>(m_Info.resources.bufferResources.size());
        }
        m_Info.resources.bufferResources.push_back(resource);
    }

    ShaderResourceBuilder::ShaderResourceBuilder(std::string_view name, ShaderResourceType type, size_t length)
    {
        m_Resource.name = name;
        m_Resource.type = type;
        m_Resource.length = length;
        m_Resource.layout.layoutType = ShaderResourceMemoryLayout::STD430;
        m_Resource.layout.binding = SHADER_ALL_BINDINGS;
    }

    ShaderResourceBuilder& ShaderResourceBuilder::SetName(std::string_view name)
    {
        m_Resource.name = name;
        return *this;
    }

    ShaderResourceBuilder& ShaderResourceBuilder::SetType(ShaderResourceType type)
    {
        m_Resource.type = type;
        return *this;
    }

    ShaderResourceBuilder& ShaderResourceBuilder::SetLength(size_t length)
    {
        m_Resource.length = length;
        return *this;
    }

    ShaderResourceBuilder& ShaderResourceBuilder::SetLayoutType(ShaderResourceMemoryLayout layout)
    {
        m_Resource.layout.layoutType = layout;
        return *this;
    }

    ShaderResourceBuilder& ShaderResourceBuilder::SetStride(size_t stride)
    {
        m_Resource.stride = stride;
        return *this;
    }

    ShaderResourceBuilder& ShaderResourceBuilder::SetBinding(uint32_t binding)
    {
        m_Resource.layout.binding = binding;
        return *this;
    }

    ShaderResourceBuilder& ShaderResourceBuilder::AddAttribute(std::string_view name, ShaderResourceAttributeType type)
    {
        m_Resource.attributes.push_back(ShaderResourceAttribute{.name = name, .type = type});
        m_Resource.stride += Shader::GetResourceAttributeTypeSize(type);

        return *this;
    }

    ShaderResourceBuilder& ShaderResourceBuilder::AddAttributes(
            std::vector<std::pair<std::string_view, ShaderResourceAttributeType>>&& attributes)
    {
        for (auto& [name, type]: attributes) { AddAttribute(name, type); }
        return *this;
    }

    ShaderResource ShaderResourceBuilder::Build()
    {
        if (m_Resource.length == 0) { m_Resource.length = 1; }
        if (m_Resource.layout.layoutType == ShaderResourceMemoryLayout::None)
        {
            m_Resource.layout.layoutType = ShaderResourceMemoryLayout::STD430;
        }

        return m_Resource;
    }

}// namespace LunaraEngine