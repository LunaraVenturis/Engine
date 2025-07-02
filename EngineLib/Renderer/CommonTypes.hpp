#pragma once
#include <optional>
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <filesystem>

namespace LunaraEngine
{
    enum class ShaderType
    {
        None = 0,
        Vertex,
        Fragment,
        Compute,
        Count,
        FlatQuad,
        FlatInstanced
    };

    struct ShaderTypeInfo;

    struct FlatInstancedShaderInfo {
        size_t numInstances{};
    };

    enum class ShaderResourceType
    {
        None = 0,
        Texture,
        Sampler,
        PushConstant,
        UniformBuffer,
        StorageBuffer,
    };


    enum class ShaderResourceFormatT
    {
        None = 0,
        R8,
        R16,
        R32,
        R8G8,
        R16G16,
        R32G32,
        R8G8B8,
        R16G16B16,
        R32G32B32,
        R8G8B8A8,
        R16G16B16A16,
        R32G32B32A32
    };

    enum class ShaderResourceDataTypeT
    {
        None = 0,
        SFloat,
        SInt,
        UInt,
        SNorm,
        UNorm
    };

    enum class ShaderResourceAttributeType : size_t
    {
        None = 0,
        Float,
        Vec2,
        Vec3,
        Vec4,
        UInt,
        Int,
        IVec2,
        IVec3,
        IVec4,
        Mat2,
        Mat3,
        Mat4
    };
    using ShaderBindingT = uint32_t;
    using ShaderLocationT = uint32_t;

    constexpr auto SHADER_ALL_BINDINGS = UINT32_MAX;
    enum class ShaderResourceMemoryLayout
    {
        None = 0,
        STD140,
        STD430
    };

    struct ShaderResourceLayout {
        ShaderBindingT binding;
        ShaderResourceMemoryLayout layoutType = ShaderResourceMemoryLayout::None;
    };

    struct ShaderResourceAttribute {
        std::string_view name;
        ShaderResourceAttributeType type;
    };

    enum ShaderResourceProperty
    {
        None = 0,
        ReadOnly = 1,
        WriteOnly = 2,
        ReadWrite = 3
    };

    struct ShaderResource {
        ShaderResourceType type{ShaderResourceType::None};
        std::string_view name{"UNKNOWN_RESOURCE"};
        size_t length{};
        size_t stride{};
        ShaderResourceLayout layout{
                ShaderResourceLayout{.binding = 0, .layoutType = ShaderResourceMemoryLayout::STD430}};
        std::vector<ShaderResourceAttribute> attributes;
        ShaderResourceProperty property{ShaderResourceProperty::ReadWrite};
    };

    struct ShaderInputResource {
        std::string_view name;
        ShaderBindingT binding;
        ShaderLocationT location;
        ShaderResourceFormatT format;
        ShaderResourceDataTypeT type;
        uint32_t offset;
    };

    struct ShaderResources {
        std::vector<ShaderResource> bufferResources;
        std::vector<ShaderInputResource> inputResources;
    };

    struct ShaderInfo {
        std::filesystem::path path;
        std::wstring name;
        bool isComputeShader = false;
        ShaderResources resources;
        uint32_t numInstances{};
    };

    struct RendererDataType;
}// namespace LunaraEngine