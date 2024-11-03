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
 * Buffer class implementation
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "Buffer.hpp"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Type definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Static variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Static function Prototypes
***********************************************************************************************************************/

/***********************************************************************************************************************
Buffer Class Implementation
***********************************************************************************************************************/

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
        if (Data) delete[] Data;
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
        if (Data) { delete[] Data; }
        Data = nullptr;

        if (size == 0) return;

        Data = new uint8_t[size];
        Size = size;
    }

    void Buffer::Release()
    {
        delete[] Data;
        Data = nullptr;
        Size = 0;
    }

    void Buffer::ZeroInitialize() {}

    uint8_t* Buffer::ReadBytes(uint32_t size, uint32_t offset)
    {
        assert(offset + size <= Size);
        uint8_t* buffer = new uint8_t[size];
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