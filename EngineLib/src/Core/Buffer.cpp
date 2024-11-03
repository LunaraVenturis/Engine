/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * @section DESCRIPTION
 * 
 * Buffer class implementation
 */

#include "Buffer.hpp"
#include <Core/Allocator.hpp>

namespace Engine
{

    Buffer::Buffer() : Data(nullptr), Size(0) {}

    Buffer::Buffer(uint8_t* data, uint32_t size) : Data(data), Size(size) {}

    Buffer::Buffer(const Buffer& other)
    {
        Data = other.Data;
        Size = other.Size;
    }

    Buffer::~Buffer()
    {
        if (Data) Allocator::DeallocateArray(Data);
    }

    Buffer Buffer::Copy(const uint8_t* data, uint32_t size)
    {
        Buffer buffer;
        buffer.Allocate(size);
        memcpy(buffer.Data, data, size);
        return buffer;
    }

    void Buffer::Allocate(uint32_t size)
    {
        if (Data) { Allocator::DeallocateArray(Data); }
        Data = nullptr;

        if (size == 0) return;

        Data = Allocator::AllocateArray<uint8_t>(size);
        Size = size;
    }

    void Buffer::Release()
    {
        Allocator::DeallocateArray(Data);
        Data = nullptr;
        Size = 0;
    }

    void Buffer::ZeroInitialize()
    {
        for (size_t i = 0; i < Size; i++) { Data[i] = 0; }
    }

    uint8_t* Buffer::ReadBytes(uint32_t size, uint32_t offset)
    {
        assert(offset + size <= Size);
        uint8_t* buffer = Allocator::AllocateArray<uint8_t>(size);
        memcpy(buffer, (uint8_t*) Data + offset, size);
        return buffer;
    }

    void Buffer::Write(uint8_t* data, uint32_t size, uint32_t offset)
    {
        assert(offset + size <= Size);
        memcpy((uint8_t*) Data + offset, data, size);
    }

    Buffer::operator bool() const { return Data; }

    uint8_t& Buffer::operator[](int index) { return ((uint8_t*) Data)[index]; }

    uint8_t Buffer::operator[](int index) const { return ((uint8_t*) Data)[index]; }

    uint32_t Buffer::GetSize() const { return Size; }
}// namespace Engine