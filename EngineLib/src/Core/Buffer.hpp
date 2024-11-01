#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section LICENSE
 * MIT License
 * 
 * Copyright (c) 2024 Christian and Krusto
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @section DESCRIPTION
 * 
 * Buffer class definition
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include <cassert>
#include <cstdint>

#include "Log.hpp"
#include "Ref.hpp"

/***********************************************************************************************************************
Class definitions
***********************************************************************************************************************/
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