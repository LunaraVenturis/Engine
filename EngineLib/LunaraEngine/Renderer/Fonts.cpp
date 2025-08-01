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
 * Copyright (c) 2025 Krusto
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
 * Fonts declarations
 */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include <LunaraEngine/Core/Memory.h>
#include "Fonts.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace LunaraEngine
{

    FontResultType LoadFont(const char* name, uint32_t size, Font* font)
    {
        (void) name;
        (void) size;
        (void) font;
        return FONT_RESULT_SUCCESS;
    }

    FontResultType FreeFont(Font* font)
    {
        (void) font;
        return FONT_RESULT_NOT_FOUND;
    }

    /*
    inline static void FontAtlasPushFont(FontAtlas* atlas, Font* font)
    {
        (void) atlas;
        (void) font;
    }
*/
    FontResultType FontAtlasLoadFont(const char* name, uint32_t size, FontType type, FontAtlas* atlas)
    {
        (void) name;
        (void) size;
        (void) type;
        (void) atlas;
        return FONT_RESULT_SUCCESS;
    }

    FontResultType FontAtlasFreeFont(FontAtlas* atlas, const char* name, FontType type)
    {
        (void) atlas;
        (void) name;
        (void) type;
        return FONT_RESULT_NOT_FOUND;
    }

    Font* FontAtlasGetFont(FontAtlas* atlas, const char* name, FontType type)
    {
        (void) atlas;
        (void) name;
        (void) type;
        return NULL;
    }

}// namespace LunaraEngine