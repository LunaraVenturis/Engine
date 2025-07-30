#include "Shader.hpp"
#include <LunaraEngine/Renderer/Vulkan/VulkanRendererAPI.hpp>
#include <LunaraEngine/Renderer/Vulkan/Shader.hpp>
#include <LunaraEngine/Core/Log.h>

namespace LunaraEngine
{
    namespace FlatInstancedShader
    {
        inline static std::shared_ptr<Shader> Create(std::filesystem::path assetsDirectory,
                                                     FlatInstancedShaderInfo* info)
        {

            return Shader::Create(
                    ShaderInfoBuilder("FlatInstanced", assetsDirectory / "Shaders/output", info->numInstances)
                            .AddVertexInputResource({{"Position", BufferResourceAttributeType::Vec2},
                                                     {"Color", BufferResourceAttributeType::Vec3}})
                            .AddResource(BufferResourceBuilder("UniformBuffer", BufferResourceType::UniformBuffer)
                                                 .AddAttribute("model", BufferResourceAttributeType::Mat4)
                                                 .AddAttribute("view", BufferResourceAttributeType::Mat4)
                                                 .AddAttribute("projection", BufferResourceAttributeType::Mat4)
                                                 .AddAttribute("zoom", BufferResourceAttributeType::Float)
                                                 .Build())
                            .Build());
        }
    }// namespace FlatInstancedShader

    namespace FlatShader
    {
        inline static std::shared_ptr<Shader> Create(std::filesystem::path assetsDirectory)
        {
            return Shader::Create(
                    ShaderInfoBuilder(L"FlatQuad", assetsDirectory / "Shaders/output")
                            .AddVertexInputResource({{"Position", BufferResourceAttributeType::Vec2},
                                                     {"Color", BufferResourceAttributeType::Vec3}})
                            .AddResource(BufferResourceBuilder("UniformBuffer", BufferResourceType::UniformBuffer)
                                                 .AddAttribute("model", BufferResourceAttributeType::Mat4)
                                                 .AddAttribute("view", BufferResourceAttributeType::Mat4)
                                                 .AddAttribute("projection", BufferResourceAttributeType::Mat4)
                                                 .AddAttribute("zoom", BufferResourceAttributeType::Float)
                                                 .Build())
                            .Build());
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

    size_t Shader::GetFormatSize(BufferResourceFormatT format)
    {
        switch (format)
        {
            case BufferResourceFormatT::R8:
                return 1;
            case BufferResourceFormatT::R16:
            case BufferResourceFormatT::R8G8:
                return 2;
            case BufferResourceFormatT::R8G8B8:
                return 3;
            case BufferResourceFormatT::R32:
            case BufferResourceFormatT::R16G16:
            case BufferResourceFormatT::R8G8B8A8:
                return 4;
            case BufferResourceFormatT::R16G16B16:
                return 6;
            case BufferResourceFormatT::R32G32:
            case BufferResourceFormatT::R16G16B16A16:
                return 8;
            case BufferResourceFormatT::R32G32B32:
                return 12;
            case BufferResourceFormatT::R32G32B32A32:
                return 16;
            default:
                return 0;
        }
    }

    size_t Shader::GetTypeSize(BufferResourceDataTypeT type)
    {
        switch (type)
        {
            case BufferResourceDataTypeT::SFloat:
                return 4;
            case BufferResourceDataTypeT::SInt:
            case BufferResourceDataTypeT::SNorm:
                return 4;
            case BufferResourceDataTypeT::UInt:
            case BufferResourceDataTypeT::UNorm:
                return 4;
            default:
                return 0;
        }
    }

    std::pair<BufferResourceFormatT, BufferResourceDataTypeT>
    Shader::GetDefaultFormatType(BufferResourceAttributeType type)
    {
        switch (type)
        {
            case BufferResourceAttributeType::Float:
                return std::make_pair(BufferResourceFormatT::R32, BufferResourceDataTypeT::SFloat);
            case BufferResourceAttributeType::Int:
                return std::make_pair(BufferResourceFormatT::R32, BufferResourceDataTypeT::SInt);
            case BufferResourceAttributeType::UInt:
                return std::make_pair(BufferResourceFormatT::R32, BufferResourceDataTypeT::UInt);
            case BufferResourceAttributeType::Vec2:
                return std::make_pair(BufferResourceFormatT::R32G32, BufferResourceDataTypeT::SFloat);
            case BufferResourceAttributeType::Vec3:
                return std::make_pair(BufferResourceFormatT::R32G32B32, BufferResourceDataTypeT::SFloat);
            case BufferResourceAttributeType::Vec4:
                return std::make_pair(BufferResourceFormatT::R32G32B32A32, BufferResourceDataTypeT::SFloat);
            case BufferResourceAttributeType::IVec2:
                return std::make_pair(BufferResourceFormatT::R32G32, BufferResourceDataTypeT::SInt);
            case BufferResourceAttributeType::IVec3:
                return std::make_pair(BufferResourceFormatT::R32G32B32, BufferResourceDataTypeT::SInt);
            case BufferResourceAttributeType::Mat2:
                return std::make_pair(BufferResourceFormatT::R32G32B32A32, BufferResourceDataTypeT::SFloat);
            case BufferResourceAttributeType::Mat3:
                return std::make_pair(BufferResourceFormatT::R32G32B32A32, BufferResourceDataTypeT::SFloat);
            case BufferResourceAttributeType::Mat4:
                return std::make_pair(BufferResourceFormatT::R32G32B32A32, BufferResourceDataTypeT::SFloat);
            default:
                return std::make_pair(BufferResourceFormatT::R32, BufferResourceDataTypeT::SFloat);
        }
    }

    size_t Shader::GetInputResourceSize(const ShaderInputResource& resource)
    {
        auto formatSize = GetFormatSize(resource.format);
        return formatSize;
    }

    std::string Shader::GetBufferResourceFormat(BufferResourceFormatT format, BufferResourceDataTypeT type)
    {
        switch (format)
        {
            case BufferResourceFormatT::R8:
                if (type == BufferResourceDataTypeT::SFloat) return "R8_SRGB";
                else
                    return "R8_UNORM";
            case BufferResourceFormatT::R16:
                if (type == BufferResourceDataTypeT::SFloat) return "R16_SFLOAT";
                else
                    return "R16_UNORM";
            case BufferResourceFormatT::R32:
                if (type == BufferResourceDataTypeT::SFloat) return "R32_SFLOAT";
                else
                    return "R32_UINT";
            case BufferResourceFormatT::R8G8:
                if (type == BufferResourceDataTypeT::SFloat) return "R8G8_SRGB";
                else
                    return "R8G8_UNORM";
            case BufferResourceFormatT::R16G16:
                if (type == BufferResourceDataTypeT::SFloat) return "R16G16_SFLOAT";
                else
                    return "R16G16_UNORM";
            case BufferResourceFormatT::R32G32:
                if (type == BufferResourceDataTypeT::SFloat) return "R32G32_SFLOAT";
                else
                    return "R32G32_UINT";
            case BufferResourceFormatT::R8G8B8:
                if (type == BufferResourceDataTypeT::SFloat) return "R8G8B8_SRGB";
                else
                    return "R8G8B8_UNORM";
            case BufferResourceFormatT::R16G16B16:
                if (type == BufferResourceDataTypeT::SFloat) return "R16G16B16_SFLOAT";
                else
                    return "R16G16B16_UNORM";
            case BufferResourceFormatT::R32G32B32:
                if (type == BufferResourceDataTypeT::SFloat) return "R32G32B32_SFLOAT";
                else
                    return "R32G32B32_UINT";
            case BufferResourceFormatT::R8G8B8A8:
                if (type == BufferResourceDataTypeT::SFloat) return "R8G8B8A8_SRGB";
                else
                    return "R8G8B8A8_UNORM";
            case BufferResourceFormatT::R16G16B16A16:
                if (type == BufferResourceDataTypeT::SFloat) return "R16G16B16A16_SFLOAT";
                else
                    return "R16G16B16A16_UNORM";
            case BufferResourceFormatT::R32G32B32A32:
                if (type == BufferResourceDataTypeT::SFloat) return "R32G32B32A32_SFLOAT";
                else
                    return "R32G32B32A32_UINT";
            default:
                return "";
        }
    }

    std::string Shader::GetBufferResourceType(BufferResourceFormatT format, BufferResourceDataTypeT type)
    {
        switch (format)
        {
            case BufferResourceFormatT::R8:
                if (type == BufferResourceDataTypeT::SFloat) return "float";
                else
                    return "unorm";
            case BufferResourceFormatT::R16:
                if (type == BufferResourceDataTypeT::SFloat) return "float";
                else
                    return "unorm";
            case BufferResourceFormatT::R32:
                if (type == BufferResourceDataTypeT::SFloat) return "float";
                else
                    return "uint";
            case BufferResourceFormatT::R8G8:
                if (type == BufferResourceDataTypeT::SFloat) return "vec2";
                else if (type == BufferResourceDataTypeT::UInt)
                    return "uivec2";
                else
                    return "ivec2";
            case BufferResourceFormatT::R16G16:
                if (type == BufferResourceDataTypeT::SFloat) return "vec2";
                else if (type == BufferResourceDataTypeT::UInt)
                    return "uivec2";
                else
                    return "ivec2";
            case BufferResourceFormatT::R32G32:
                if (type == BufferResourceDataTypeT::SFloat) return "vec2";
                else if (type == BufferResourceDataTypeT::UInt)
                    return "uivec2";
                else
                    return "ivec2";
            case BufferResourceFormatT::R8G8B8:
                if (type == BufferResourceDataTypeT::SFloat) return "vec3";
                else if (type == BufferResourceDataTypeT::UInt)
                    return "uivec3";
                else
                    return "ivec3";
            case BufferResourceFormatT::R16G16B16:
                if (type == BufferResourceDataTypeT::SFloat) return "vec3";
                else if (type == BufferResourceDataTypeT::UInt)
                    return "uivec3";
                else
                    return "ivec3";
            case BufferResourceFormatT::R32G32B32:
                if (type == BufferResourceDataTypeT::SFloat) return "vec3";
                else if (type == BufferResourceDataTypeT::UInt)
                    return "uivec3";
                else
                    return "ivec3";
            case BufferResourceFormatT::R8G8B8A8:
                if (type == BufferResourceDataTypeT::SFloat) return "vec4";
                else if (type == BufferResourceDataTypeT::UInt)
                    return "uivec4";
                else
                    return "ivec4";
            case BufferResourceFormatT::R16G16B16A16:
                if (type == BufferResourceDataTypeT::SFloat) return "vec4";
                else if (type == BufferResourceDataTypeT::UInt)
                    return "uivec4";
                else
                    return "ivec4";
            case BufferResourceFormatT::R32G32B32A32:
                if (type == BufferResourceDataTypeT::SFloat) return "vec4";
                else if (type == BufferResourceDataTypeT::UInt)
                    return "uivec4";
                else
                    return "ivec4";
            default:
                return "";
        }
    }

    size_t Shader::GetResourceAttributeTypeSize(BufferResourceAttributeType type)
    {
        switch (type)
        {
            case BufferResourceAttributeType::Float:
                return sizeof(float);
            case BufferResourceAttributeType::Vec2:
                return sizeof(float) * 2;
            case BufferResourceAttributeType::Vec3:
                return sizeof(float) * 3;
            case BufferResourceAttributeType::Vec4:
                return sizeof(float) * 4;
            case BufferResourceAttributeType::Mat2:
                return sizeof(float) * 4;
            case BufferResourceAttributeType::Mat3:
                return sizeof(float) * 9;
            case BufferResourceAttributeType::Mat4:
                return sizeof(float) * 16;
            case BufferResourceAttributeType::UInt:
                return sizeof(uint32_t);
            case BufferResourceAttributeType::Int:
                return sizeof(int32_t);
            case BufferResourceAttributeType::IVec2:
                return sizeof(int32_t) * 2;
            case BufferResourceAttributeType::IVec3:
                return sizeof(int32_t) * 3;
            case BufferResourceAttributeType::IVec4:
                return sizeof(int32_t) * 4;
            default:
                return 0;
        }
    }

    ShaderInfoBuilder::ShaderInfoBuilder(std::filesystem::path name, std::filesystem::path path, size_t numInstances,
                                         bool isCompute)
    {
        m_Info.name = name.wstring();
        m_Info.path = path;
        m_Info.numInstances = (uint32_t) numInstances;
        m_Info.isComputeShader = isCompute;
    }

    ShaderInfoBuilder& ShaderInfoBuilder::SetName(std::filesystem::path name)
    {
        m_Info.name = name.wstring();
        return *this;
    }

    ShaderInfoBuilder& ShaderInfoBuilder::SetPath(std::filesystem::path path)
    {
        m_Info.path = path;
        return *this;
    }

    ShaderInfoBuilder& ShaderInfoBuilder::UseAsComputeShader()
    {
        m_Info.isComputeShader = true;
        return *this;
    }

    ShaderInfoBuilder& ShaderInfoBuilder::AddVertexInputResource(
            std::vector<std::pair<std::string_view, BufferResourceAttributeType>>&& resources)
    {
        const ShaderBinding binding = ShaderBinding::_0;
        uint32_t location{};
        uint32_t offset{};
        for (auto& [name, type]: resources)
        {
            auto [BufferResourceFormat, BufferResourceType] = Shader::GetDefaultFormatType(type);

            m_Info.resources.inputResources.push_back(ShaderInputResource{.name = name,
                                                                          .binding = binding,
                                                                          .location = location,
                                                                          .format = BufferResourceFormat,
                                                                          .type = BufferResourceType,
                                                                          .offset = offset});
            offset += Shader::GetFormatSize(BufferResourceFormat);
            ++location;
        }
        return *this;
    }

    ShaderInfoBuilder& ShaderInfoBuilder::SetNumInstances(uint32_t numInstances)
    {
        m_Info.numInstances = numInstances;
        return *this;
    }

    ShaderInfoBuilder& ShaderInfoBuilder::AddVertexInputResource(const ShaderInputResource& resource)
    {
        m_Info.resources.inputResources.push_back(resource);
        return *this;
    }

    ShaderInfoBuilder& ShaderInfoBuilder::AddResource(BufferResource resource)
    {
        if (resource.layout.binding == ShaderBinding::ALL)
        {
            resource.layout.binding = static_cast<ShaderBinding>(m_SetCurrentBinding[m_CurrentSet]++);
        }
        m_Info.resources.bufferResources.push_back(resource);
        return *this;
    }

    ShaderInfoBuilder& ShaderInfoBuilder::AddResource(TextureResource resource)
    {
        resource.layout.set = ++m_CurrentSet;
        if (resource.layout.binding == ShaderBinding::ALL)
        {
            resource.layout.binding = static_cast<ShaderBinding>(m_SetCurrentBinding[m_CurrentSet]++);
        }
        m_Info.resources.textureResources.push_back(resource);
        return *this;
    }

    ShaderInfoBuilder& ShaderInfoBuilder::AddResources(std::vector<BufferResource>&& resources)
    {
        for (auto& resource: resources) { AddResource(resource); }
        return *this;
    }

    ShaderInfoBuilder& ShaderInfoBuilder::AddResources(std::vector<TextureResource>&& resources)
    {
        for (auto& resource: resources) { AddResource(resource); }
        return *this;
    }

    BufferResourceBuilder::BufferResourceBuilder(std::string_view name, BufferResourceType type, size_t length)
    {
        m_Resource.name = name;
        m_Resource.type = type;
        m_Resource.length = length;
        m_Resource.layout.layoutType = BufferResourceMemoryLayout::STD430;
        m_Resource.layout.binding = ShaderBinding::ALL;
    }

    BufferResourceBuilder& BufferResourceBuilder::SetName(std::string_view name)
    {
        m_Resource.name = name;
        return *this;
    }

    BufferResourceBuilder& BufferResourceBuilder::SetType(BufferResourceType type)
    {
        m_Resource.type = type;
        return *this;
    }

    BufferResourceBuilder& BufferResourceBuilder::SetLength(size_t length)
    {
        m_Resource.length = length;
        return *this;
    }

    BufferResourceBuilder& BufferResourceBuilder::SetLayoutType(BufferResourceMemoryLayout layout)
    {
        m_Resource.layout.layoutType = layout;
        return *this;
    }

    BufferResourceBuilder& BufferResourceBuilder::SetStride(size_t stride)
    {
        m_Resource.stride = stride;
        return *this;
    }

    BufferResourceBuilder& BufferResourceBuilder::SetBinding(ShaderBinding binding)
    {
        m_Resource.layout.binding = binding;
        return *this;
    }

    BufferResourceBuilder& BufferResourceBuilder::AddAttribute(std::string_view name, BufferResourceAttributeType type)
    {
        m_Resource.attributes.push_back(BufferResourceAttribute{.name = name, .type = type});
        m_Resource.stride += Shader::GetResourceAttributeTypeSize(type);

        return *this;
    }

    BufferResourceBuilder& BufferResourceBuilder::AddAttributes(
            std::vector<std::pair<std::string_view, BufferResourceAttributeType>>&& attributes)
    {
        for (auto& [name, type]: attributes) { AddAttribute(name, type); }
        return *this;
    }

    BufferResource BufferResourceBuilder::Build()
    {
        if (m_Resource.length == 0) { m_Resource.length = 1; }
        if (m_Resource.layout.layoutType == BufferResourceMemoryLayout::None)
        {
            m_Resource.layout.layoutType = BufferResourceMemoryLayout::STD430;
        }

        return m_Resource;
    }


}// namespace LunaraEngine