#pragma once
#include <cstdint>

namespace LunaraEngine
{

    using std::size_t;

    template <typename T = uint16_t>
    class IndexBuffer
    {
    public:
        IndexBuffer() = default;

        IndexBuffer(T* data, size_t length, size_t stride = sizeof(T));
        IndexBuffer(size_t length, size_t stride = sizeof(T));
        ~IndexBuffer();

    public:
        void Create(T* data, size_t length, size_t stride = sizeof(T));
        void Upload(T* data, size_t length, size_t stride = sizeof(T));

        [[nodiscard]] size_t GetSize() const { return m_Size; }

        [[nodiscard]] size_t GetStride() const { return m_Stride; }

        [[nodiscard]] size_t GetLength() const { return m_Size / m_Stride; }

    public:
        [[nodiscard]] IndexBuffer* GetHandle() const { return m_Handle; }

    private:
        IndexBuffer* m_Handle{};
        size_t m_Size{};
        size_t m_Stride{};
    };
}// namespace LunaraEngine

#include <LunaraEngine/Renderer/Buffer/IndexBuffer.inl>