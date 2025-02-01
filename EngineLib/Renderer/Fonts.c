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
#include "Core/Memory.h"
#include "Fonts.h"
#include "Renderer.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions declarations
************************************************************************************************************************/

FontResultType LoadFont(const char* name, uint32_t size, Font* font)
{

    TTF_Font* font_ptr = TTF_OpenFont(name, (float) size);
    if (font_ptr == NULL) { return FONT_RESULT_NOT_FOUND; }

    font->name = name;
    font->size = size;
    font->data = font_ptr;
    font->type = FONT_NORMAL;
    return FONT_RESULT_SUCCESS;
}

FontResultType FreeFont(Font* font)
{

    if (font->data != NULL)
    {
        TTF_CloseFont((TTF_Font*) font->data);
        return FONT_RESULT_SUCCESS;
    }
    return FONT_RESULT_NOT_FOUND;
}

inline static void FontAtlasPushFont(FontAtlas* atlas, Font* font)
{

    FontNode* node = ENG_MALLOC(sizeof(FontNode));
    node->next = NULL;
    node->data = font;

    if (atlas->fonts.head == NULL)
    {
        atlas->fonts.head = node;
        atlas->fonts.current = node;
    }
    else { atlas->fonts.current->next = (Font*) node; }
    atlas->fonts.current = node;
}

FontResultType FontAtlasLoadFont(const char* name, uint32_t size, FontType type, FontAtlas* atlas)
{
    Font* font = FontAtlasGetFont(atlas, name, type);
    if (font == NULL)
    {
        font = ENG_MALLOC(sizeof(Font));
        FontResultType result = LoadFont(name, size, font);
        if (result != FONT_RESULT_SUCCESS) { return result; }
        FontAtlasPushFont(atlas, font);
    }
    return FONT_RESULT_SUCCESS;
}

FontResultType FontAtlasFreeFont(FontAtlas* atlas, const char* name, FontType type)
{
    FontNode* node = atlas->fonts.head;
    FontNode* prev = NULL;
    while (node != NULL)
    {
        if (strcmp(node->data->name, name) == 0 && node->data->type == type)
        {
            FreeFont(node->data);
            FontNode* tmp = (FontNode*) node->next;
            free(node);
            prev->next = (Font*) tmp;
            if (tmp) { node = tmp; }
            return FONT_RESULT_SUCCESS;
        }
        else
        {
            prev = node;
            node = (FontNode*) node->next;
        }
    }
    return FONT_RESULT_NOT_FOUND;
}

Font* FontAtlasGetFont(FontAtlas* atlas, const char* name, FontType type)
{
    FontNode* node = atlas->fonts.head;
    while (node != NULL)
    {
        if (strcmp(node->data->name, name) == 0 && node->data->type == type) { return node->data; }
        else { node = (FontNode*) node->next; }
    }
    return NULL;
}
