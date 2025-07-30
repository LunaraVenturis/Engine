#pragma once
#include <cstdint>

namespace LunaraEngine
{

    using std::size_t;

    template <typename T = uint16_t>
    class StorageBuffer
    {
    public:
        StorageBuffer() = default;

        StorageBuffer(T* data, size_t length, size_t stride = sizeof(T));
        StorageBuffer(size_t length, size_t stride = sizeof(T));
        ~StorageBuffer();

    public:
        void Create(T* data, size_t length, size_t stride = sizeof(T));
        void Upload(T* data, size_t length, size_t stride = sizeof(T));

        [[nodiscard]] size_t GetSize() const { return m_Size; }

        [[nodiscard]] size_t GetStride() const { return m_Stride; }

        [[nodiscard]] size_t GetLegnth() const { return m_Size / m_Stride; }

    public:
        [[nodiscard]] StorageBuffer* GetHandle() const { return m_Handle; }

    private:
        StorageBuffer* m_Handle{};
        size_t m_Size{};
        size_t m_Stride{};
    };
}// namespace LunaraEngine

#include <LunaraEngine/Renderer/Buffer/StorageBuffer.inl>