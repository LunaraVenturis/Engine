/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com ) 
 * @coauthor Neyko Naydenov (neyko641@gmail.com)
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section LICENSE
 * MIT License
 * 
 * Copyright (c) 2025 Krusto, Neyko
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
 * Renderer declarations
 */

#pragma once

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "VulkanDataTypes.hpp"

namespace LunaraEngine
{

    class VulkanRendererCommand
    {
    public:
        VulkanRendererCommand() = delete;
        ~VulkanRendererCommand() = delete;

    public:
        static void BindShader(RendererDataType* rendererData, const RendererCommand* command);
        static void DrawQuad(RendererDataType* rendererData, const RendererCommand* command);
        static void Clear(RendererDataType* rendererData, const RendererCommand* command);
        static void DrawIndexed(RendererDataType* rendererData, const RendererCommand* command);
        static void BeginRenderPass(RendererDataType* rendererData, const RendererCommand* command);
        static void EndRenderPass(RendererDataType* rendererData, const RendererCommand* command);
        static void BeginFrame(RendererDataType* rendererData, const RendererCommand* command);
        static void Present(RendererDataType* rendererData, const RendererCommand* command);
        static void Nop(RendererDataType* rendererData, const RendererCommand* command);
    };

}// namespace LunaraEngine