/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * Buffer class templated functions interface
 */

#pragma once

#include "Buffer.hpp"

namespace Engine
{

    template <typename T>
    T& Buffer::Read(uint32_t offset)
    {
        return *(T*) ((uint8_t*) Data + offset);
    }

    template <typename T>
    T& Buffer::Read(uint32_t offset) const
    {
        return *(T*) ((uint8_t*) Data + offset);
    }

    template <typename T>
    T* Buffer::As()
    {
        return (T*) Data;
    }

}// namespace Engine