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
#include <memory>

namespace LunaraEngine
{

    class VulkanRendererAPI: public RendererAPI
    {
    public:
        VulkanRendererAPI() = default;
        ~VulkanRendererAPI() = default;
        VulkanRendererAPI(const VulkanRendererAPI& other) = delete;
        VulkanRendererAPI& operator=(const VulkanRendererAPI&) = delete;
        VulkanRendererAPI(VulkanRendererAPI&& other) = delete;
        VulkanRendererAPI& operator=(VulkanRendererAPI&& other) = delete;

    public:
        virtual Window* GetWindow() override;
        virtual void Present() override;
        virtual void Init(const RendererAPIConfig& config) override;
        virtual void Destroy() override;

    private:
        void CreateWindow();

    private:
        VkInstance* GetVkInstance() { return &m_RendererData->instance; }

        VkDevice* GetDevice() { return &m_RendererData->device; }

        VkPhysicalDevice* GetPhysicalDevice() { return &m_RendererData->physicalDevice; }

    private:
        std::unique_ptr<RendererDataType> m_RendererData;
        RendererAPIConfig m_Config;
    };

}// namespace LunaraEngine