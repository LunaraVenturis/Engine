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

#ifndef ENG_Texture
#define ENG_Texture

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "Core/STDTypes.h"
#ifdef __cplusplus
extern "C" {
#endif
/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Type definitions
***********************************************************************************************************************/
typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
} TextureInfo;

typedef enum
{
    TextureResult_None = 0,
    TextureResult_Success,
    TextureResult_Error
} TextureResultType;

typedef struct {
    TextureInfo info;
    void* data;
} Texture;

/***********************************************************************************************************************
Functions declarations
************************************************************************************************************************/

extern TextureResultType Texture_Init(Texture* texture, uint32_t width, uint32_t height, uint32_t channels);
extern TextureResultType Texture_SetData(Texture* texture, void* data);
extern TextureResultType Texture_GetInfo(Texture* texture, TextureInfo** info);
extern TextureResultType Texture_Destroy(Texture* texture);

#ifdef __cplusplus
}
#endif
#endif