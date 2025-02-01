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

#ifndef ENG_Fonts
#define ENG_Fonts

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "Core/STDTypes.h"

#ifdef __cplusplus
namespace LunaraEngine
{
    extern "C" {
#endif
    /***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

    /***********************************************************************************************************************
Type definitions
***********************************************************************************************************************/
    typedef enum
    {
        FONT_RESULT_NONE = 0,
        FONT_RESULT_SUCCESS,
        FONT_RESULT_NOT_FOUND,
        FONT_RESULT_ERROR
    } FontResultType;

    typedef enum
    {
        FONT_NORMAL = 0,
        FONT_BOLD,
        FONT_ITALIC,
        FONT_BOLD_ITALIC
    } FontType;

    typedef struct {
        uint32_t size;
        const char* name;
        void* data;
        FontType type;
    } Font;

    typedef struct {
        Font* data;
        Font* next;
    } FontNode;

    typedef struct {
        FontNode* head;
        FontNode* current;
    } FontStack;

    typedef struct {
        FontStack fonts;
        uint32_t font_count;
    } FontAtlas;

    /***********************************************************************************************************************
Functions declarations
************************************************************************************************************************/

    extern FontResultType LoadFont(const char* name, uint32_t size, Font* font);
    extern FontResultType FreeFont(Font* font);

    extern FontResultType FontAtlasLoadFont(const char* name, uint32_t size, FontType type, FontAtlas* atlas);
    extern FontResultType FontAtlasFreeFont(FontAtlas* atlas, const char* name, FontType type);
    extern Font* FontAtlasGetFont(FontAtlas* atlas, const char* name, FontType type);


#ifdef __cplusplus
    }
}
#endif
#endif