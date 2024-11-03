#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * Buffer class definition
 */

#include <cassert>
#include <cstdint>

#include "Log.hpp"
#include "Ref.hpp"

namespace Engine
{
    class Buffer: public RefCounted
    {
    public:
        Buffer();

        Buffer(uint8_t* data, uint32_t size);

        Buffer(const Buffer& other);
        ~Buffer();

    public:
        static Buffer Copy(const uint8_t* data, uint32_t size);

        void Allocate(uint32_t size);

        void Release();

        void ZeroInitialize();

        uint8_t* ReadBytes(uint32_t size, uint32_t offset);

        void Write(uint8_t* data, uint32_t size, uint32_t offset = 0);

        uint32_t GetSize() const;

    public:
        template <typename T>
        T& Read(uint32_t offset = 0);
        template <typename T>
        T& Read(uint32_t offset = 0) const;

        template <typename T>
        T* As();

    public:
        operator bool() const;

        uint8_t& operator[](int index);
        uint8_t operator[](int index) const;

    public:
        uint8_t* Data;
        uint32_t Size;
    };
}// namespace Engine

#include "Buffer.inl"