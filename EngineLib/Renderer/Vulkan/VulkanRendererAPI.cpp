/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com ) 
 * @coauthor Neyko Naydenov ( neyko641@gmail.com )
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
 * Renderer definitions
 */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "VulkanRendererAPI.hpp"
#include "Renderer/Window.hpp"
#include "Core/Log.h"
#include "VulkanInitialization.hpp"
#include "SDL3/SDL_vulkan.h"
#include "SDL3/SDL.h"
#include <stdexcept>
#include "GraphicsPipeline.hpp"

namespace LunaraEngine
{

    static std::vector<uint32_t> ReadFile(std::filesystem::path name)
    {

        std::ifstream file(name, std::ios::ate | std::ios::binary);
        if (!file.is_open()) { throw std::runtime_error("failed to open file!"); }

        std::streamsize fileSize = file.tellg();
        if (fileSize < 0) { throw std::runtime_error("failed to determine file size!"); }
        std::vector<uint32_t> buffer(static_cast<size_t>(fileSize));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();

        return buffer;
    }

    void VulkanRendererAPI::CreateWindow()
    {
        SDL_Init(SDL_INIT_VIDEO);
        m_RendererData->window = new Window;
        m_RendererData->window->name = m_Config.windowName.data();
        m_RendererData->window->data =
                static_cast<void*>(SDL_CreateWindow(m_RendererData->window->name, (int) m_Config.initialWidth,
                                                    (int) m_Config.initialHeight, SDL_WINDOW_VULKAN));
        if (m_RendererData->window->data == nullptr) { throw std::runtime_error("Couldn't create Window"); }
    }

    Window* VulkanRendererAPI::GetWindow() { return m_RendererData->window; }

    void VulkanRendererAPI::Present() { throw std::runtime_error("Not implemented"); }

    void VulkanRendererAPI::Init(const RendererAPIConfig& config)
    {
        m_Config = config;
        m_RendererData = std::make_unique<RendererDataType>();

        CreateWindow();
        VulkanInitializer::Initialize(m_RendererData.get());

        auto vertShaderCode = ReadFile(config.shadersDirectory / "output/vert.spv");
        auto fragShaderCode = ReadFile(config.shadersDirectory / "output/frag.spv");
        std::vector<VkDescriptorSetLayout> descriptorSets;
        m_RendererData->pipeline =
                new GraphicsPipeline(m_RendererData->device, vertShaderCode, fragShaderCode,
                                     m_RendererData->swapChainExtent, descriptorSets, m_RendererData->renderPass);
    }

    void VulkanRendererAPI::Destroy()
    {
        delete m_RendererData->pipeline;
        VulkanInitializer::Goodbye(m_RendererData.get());
        SDL_DestroyWindow(static_cast<SDL_Window*>(m_RendererData->window->data));
    }


}// namespace LunaraEngine