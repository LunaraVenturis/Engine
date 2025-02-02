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
 * Audio Manager declarations
 */

#pragma once

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include <string_view>

/***********************************************************************************************************************
Type Declarations
***********************************************************************************************************************/
namespace LunaraEngine
{
    typedef enum
    {
        AudioManager_Result_Success = 0,
        AudioManager_Result_Fail = 1,
    } AudioManagerResult;
}

/***********************************************************************************************************************
Class Declarations
***********************************************************************************************************************/

namespace LunaraEngine
{

    class AudioManager
    {
    public:
        AudioManager() = default;
        ~AudioManager() = default;

    public:
        static AudioManagerResult Init();
        static void LoadAudio(std::string_view name, std::string_view path);
        static void PlayAudio(std::string_view name);
        static void Destroy();
    };
}// namespace LunaraEngine