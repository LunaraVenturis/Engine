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
 * Texture declarations
 */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "Texture.h"
#include "Renderer.h"
#include <SDL3/SDL_render.h>

#ifdef __cplusplus
namespace LunaraEngine
{
#endif
    /***********************************************************************************************************************
    Macro definitions
    ***********************************************************************************************************************/

    /***********************************************************************************************************************
    Functions declarations
    ************************************************************************************************************************/

    TextureResultType Texture_Init(Texture* texture, uint32_t width, uint32_t height, uint32_t channels)
    {
        texture->info.width = (float) width;
        texture->info.height = (float) height;
        texture->info.channels = channels;
        texture->data = NULL;

        if (texture->info.width == 0 || texture->info.height == 0 || texture->info.channels == 0)
        {
            return TextureResult_Error;
        }
        SDL_PixelFormat format;
        if (channels >= 4) { format = SDL_PIXELFORMAT_RGBA8888; }
        else if (channels == 3) { format = SDL_PIXELFORMAT_XRGB8888; }
        else if (channels == 2) { format = SDL_PIXELFORMAT_RGB565; }
        else { return TextureResult_Error; }

        texture->data = (void*) SDL_CreateTexture(RendererGet()->renderer, format, SDL_TEXTUREACCESS_STATIC,
                                                  (int) width, (int) height);
        return TextureResult_Success;
    }

    TextureResultType Texture_SetData(Texture* texture, void* data)
    {
        texture->data = data;
        return TextureResult_Success;
    }

    TextureResultType Texture_GetInfo(Texture* texture, TextureInfo** info)
    {
        *info = &texture->info;
        return TextureResult_Success;
    }

    TextureResultType Texture_Destroy(Texture* texture)
    {
        SDL_DestroyTexture(texture->data);
        texture->data = NULL;
        return TextureResult_Success;
    }
#ifdef __cplusplus
}
#endif