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

    enum class BufferResourceType : size_t
    {
        None = 0,
        Texture,
        PushConstant,
        UniformBuffer,
        StorageBuffer,
        Buffer
    };
    enum class TextureResourceType
    {
        None = 0,
        Texture2D,
        Texture2DArray,
        Texture2DBindlessArray,
        Texture3D
    };

    enum class BufferResourceFormatT
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

    enum class BufferResourceDataTypeT
    {
        None = 0,
        SFloat,
        SInt,
        UInt,
        SNorm,
        UNorm
    };

    enum class BufferResourceAttributeType : size_t
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

    enum class BufferResourceMemoryLayout
    {
        None = 0,
        STD140,
        STD430
    };

    struct BufferResourceLayout {
        ShaderBinding binding = ShaderBinding::_0;
        size_t set = 0;
        BufferResourceMemoryLayout layoutType = BufferResourceMemoryLayout::None;
    };

    struct BufferResourceAttribute {
        std::string_view name;
        BufferResourceAttributeType type;
    };

    enum BufferResourceProperty
    {
        None = 0,
        ReadOnly = 1,
        WriteOnly = 2,
        ReadWrite = 3
    };

    enum class TextureDataFormat : size_t
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

    enum class TextureDataType : size_t
    {
        None = 0,
        Int,
        Float
    };

    enum class TextureFormat : size_t
    {
        None = 0,
        R,
        RG,
        RGB,
        RGBA
    };

    struct TextureInfo {
        std::filesystem::path path;
        std::wstring_view name;
        uint32_t width{};
        uint32_t height{};
        uint32_t layerCount{1};
        TextureFormat format = TextureFormat::None;
        TextureDataType type = TextureDataType::None;
        uint32_t channelDepth{8};
    };

    struct BufferResource {
        BufferResourceType type{BufferResourceType::None};
        std::string_view name{"UNKNOWN_RESOURCE"};
        size_t length{};
        size_t stride{};
        BufferResourceLayout layout{BufferResourceLayout{.binding = ShaderBinding::_0,
                                                         .set = 0,
                                                         .layoutType = BufferResourceMemoryLayout::STD430}};
        std::vector<BufferResourceAttribute> attributes;
        BufferResourceProperty property{BufferResourceProperty::ReadWrite};
    };

    struct TextureResource {
        std::filesystem::path path;
        std::vector<std::wstring_view> textureNames;
        BufferResourceType resourceType{BufferResourceType::None};
        TextureResourceType textureType{TextureResourceType::None};
        std::string_view name{"UNKNOWN_RESOURCE"};
        uint32_t width{};
        uint32_t height{};
        uint32_t layerCount{1};
        uint32_t channelDepth{8};
        TextureFormat format = TextureFormat::None;
        TextureDataType type = TextureDataType::None;
        BufferResourceLayout layout{BufferResourceLayout{.binding = ShaderBinding::_0,
                                                         .set = 0,
                                                         .layoutType = BufferResourceMemoryLayout::STD430}};
    };

    struct ShaderInputResource {
        std::string_view name;
        ShaderBinding binding;
        ShaderLocationT location;
        BufferResourceFormatT format;
        BufferResourceDataTypeT type;
        uint32_t offset;
    };

    struct BufferResources {
        std::vector<BufferResource> bufferResources;
        std::vector<TextureResource> textureResources;
        std::vector<ShaderInputResource> inputResources;
    };

    struct ShaderInfo {
        std::filesystem::path path;
        std::wstring name;
        bool isComputeShader = false;
        BufferResources resources;
        uint32_t numInstances{};
    };


    struct RendererDataType;

    enum class RenderingBasePrimitive : size_t
    {
        None = 0,
        POINTS,
        LINES,
        LINE_STRIP,
        TRIANGLES,
        TRIANGLE_STRIP,
        TRIANGLE_FAN,
        LINES_WITH_ADJACENCY,
        LINE_STRIP_WITH_ADJACENCY,
        TRIANGLES_WITH_ADJACENCY,
        TRIANGLE_STRIP_WITH_ADJACENCY,
        PATCHES
    };

    enum class PolygonMode : size_t
    {
        None = 0,
        FILL,
        LINE,
        POINT
    };
    enum class PipelineType : uint32_t
    {
        Graphics = 0,
        Compute
    };

    enum class ShaderStage : uint32_t
    {
        Vertex = 0,
        Fragment,
        Compute
    };
}// namespace LunaraEngine