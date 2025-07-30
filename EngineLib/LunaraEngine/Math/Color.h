/**
* @file
 * @author Krasto Stoyanov (k.stoianov2@gmail.com)
 * @brief
 * @version 1.0
 * @date
 *
 * @section LICENSE
 * MIT License
 *
 * Copyright (c) 2025 Neyko Naydenov
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
 * Color Declaration
 */


#ifndef COLOR_H
#define COLOR_H

#ifdef __cplusplus
extern "C" {
namespace LunaraEngine
{
#endif
    struct Color4 {
        float r;
        float g;
        float b;
        float a;
    };

    struct Color3 {
        float r;
        float g;
        float b;
    };

#ifdef __cplusplus

    constexpr Color4 COLOR_RED{1.0f, 0.0f, 0.0f, 1.0f};
}
}
#else
#define COLOR_RED (Color4{1.0f, 0.0f, 0.0f, 1.0f})
#endif
#endif//COLOR_H
