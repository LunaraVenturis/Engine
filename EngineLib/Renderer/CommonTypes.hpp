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
        Compute
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

    using ShaderBindingT = uint32_t;
    using ShaderLocationT = uint32_t;

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

    struct ShaderResource {
        ShaderResourceType type;
        std::string_view name;
        size_t size;
        ShaderResourceLayout layout;
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
    };
}// namespace LunaraEngine