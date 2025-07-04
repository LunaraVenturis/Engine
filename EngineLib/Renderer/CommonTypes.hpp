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
    using ShaderLocationT = uint32_t;

    enum class ShaderBinding : size_t
    {
        _0 = 0,
        _1,
        _2,
        _3,
        _4,
        _5,
        _6,
        _7,
        _8,
        _9,
        _10,
        _11,
        _12,
        _13,
        _14,
        _15,
        _16,
        _17,
        _18,
        _19,
        _20,
        ALL = 0xFFFFFFFF
    };

    enum class ShaderResourceMemoryLayout
    {
        None = 0,
        STD140,
        STD430
    };

    struct ShaderResourceLayout {
        ShaderBinding binding = ShaderBinding::_0;
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
                ShaderResourceLayout{.binding = ShaderBinding::_0, .layoutType = ShaderResourceMemoryLayout::STD430}};
        std::vector<ShaderResourceAttribute> attributes;
        ShaderResourceProperty property{ShaderResourceProperty::ReadWrite};
    };

    struct ShaderInputResource {
        std::string_view name;
        ShaderBinding binding;
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