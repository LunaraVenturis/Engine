#pragma once
#include <string_view>
#include <vector>
#include <cstdint>
#include <map>
#include <algorithm>
#include <ranges>

namespace LunaraEngine
{
    enum class VertexAttributeType
    {
        FLOAT,
        VEC2,
        VEC3,
        VEC4,
        INT,
        IVEC2,
        IVEC3,
        IVEC4,
        MAT2,
        MAT3,
        MAT4,
        BOOL
    };

    struct VertexAttribute {
        std::string_view name;
        VertexAttributeType type;
    };

    class VertexAttributeLayout
    {
    public:
        VertexAttributeLayout() = default;

        VertexAttributeLayout(std::initializer_list<VertexAttribute> attributes) : m_Attributes(attributes) {}

        size_t GetStride()
        {
            if (m_Stride != 0) { return m_Stride; }
            std::ranges::for_each(m_Attributes, [&](const auto& attribute) {
                m_Stride += VertexAttributeLayout::GetSize(attribute.type);
            });
            return m_Stride;
        }

        const std::vector<VertexAttribute>& GetAttributes() const { return m_Attributes; }

    private:
        std::vector<VertexAttribute> m_Attributes;
        size_t m_Stride{};

        template <VertexAttributeType T>
        struct AttributeSize;

        template <>
        struct AttributeSize<VertexAttributeType::FLOAT> {
            static constexpr size_t value = sizeof(float);
        };

        template <>
        struct AttributeSize<VertexAttributeType::VEC2> {
            static constexpr size_t value = sizeof(float) * 2;
        };

        template <>
        struct AttributeSize<VertexAttributeType::VEC3> {
            static constexpr size_t value = sizeof(float) * 3;
        };

        template <>
        struct AttributeSize<VertexAttributeType::VEC4> {
            static constexpr size_t value = sizeof(float) * 4;
        };

        template <>
        struct AttributeSize<VertexAttributeType::INT> {
            static constexpr size_t value = sizeof(int);
        };

        template <>
        struct AttributeSize<VertexAttributeType::IVEC2> {
            static constexpr size_t value = sizeof(int) * 2;
        };

        template <>
        struct AttributeSize<VertexAttributeType::IVEC3> {
            static constexpr size_t value = sizeof(int) * 3;
        };

        template <>
        struct AttributeSize<VertexAttributeType::IVEC4> {
            static constexpr size_t value = sizeof(int) * 4;
        };

        template <>
        struct AttributeSize<VertexAttributeType::MAT2> {
            static constexpr size_t value = sizeof(float) * 4;
        };

        template <>
        struct AttributeSize<VertexAttributeType::MAT3> {
            static constexpr size_t value = sizeof(float) * 9;
        };

        template <>
        struct AttributeSize<VertexAttributeType::MAT4> {
            static constexpr size_t value = sizeof(float) * 16;
        };

        template <>
        struct AttributeSize<VertexAttributeType::BOOL> {
            static constexpr size_t value = sizeof(bool);
        };

        template <VertexAttributeType T>
        static constexpr size_t GetSize()
        {
            return AttributeSize<T>::value;
        }

        static constexpr size_t GetSize(VertexAttributeType type)
        {
            switch (type)
            {
                case VertexAttributeType::FLOAT:
                    return sizeof(float);
                case VertexAttributeType::VEC2:
                    return sizeof(float) * 2;
                case VertexAttributeType::VEC3:
                    return sizeof(float) * 3;
                case VertexAttributeType::VEC4:
                    return sizeof(float) * 4;
                case VertexAttributeType::INT:
                    return sizeof(int);
                case VertexAttributeType::IVEC2:
                    return sizeof(int) * 2;
                case VertexAttributeType::IVEC3:
                    return sizeof(int) * 3;
                case VertexAttributeType::IVEC4:
                    return sizeof(int) * 4;
                case VertexAttributeType::MAT2:
                    return sizeof(float) * 4;
                case VertexAttributeType::MAT3:
                    return sizeof(float) * 9;
                case VertexAttributeType::MAT4:
                    return sizeof(float) * 16;
                case VertexAttributeType::BOOL:
                    return sizeof(bool);
                default:
                    return 0;
            }
        }
    };

    class VertexBuffer
    {
    public:
        VertexBuffer() = default;

        VertexBuffer(VertexAttributeLayout layout, size_t length);
        VertexBuffer(VertexAttributeLayout layout, uint8_t* data, size_t length);
        ~VertexBuffer() noexcept(false);

    public:
        void Create(VertexAttributeLayout layout, uint8_t* data, size_t length);
        void Upload(uint8_t* data, size_t size);

        [[nodiscard]] size_t GetSize() const { return m_Size; }

        [[nodiscard]] size_t GetStride() const { return m_Stride; }

    public:
        [[nodiscard]] VertexBuffer* GetHandle() const { return m_Handle; }

    private:
        VertexBuffer* m_Handle{};
        size_t m_Size{};
        size_t m_Stride{};
    };
}// namespace LunaraEngine