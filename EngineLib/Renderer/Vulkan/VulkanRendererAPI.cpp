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
#include <stdexcept>
#include <Core/Log.h>
#include <vulkan/vulkan.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL.h>
#include <Renderer/Vulkan/VulkanRendererAPI.hpp>
#include <Renderer/Window.hpp>
#include <Renderer/Vulkan/VulkanInitialization.hpp>
#include <Renderer/Vulkan/GraphicsPipeline.hpp>
#include <Renderer/Vulkan/IndexBuffer.hpp>
#include <Renderer/Vulkan/VertexBuffer.hpp>
#include <Renderer/Vulkan/Shader.hpp>
#include <Renderer/Vulkan/VulkanRendererCommands.hpp>
#include <Renderer/IndexBuffer.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Renderer/Shader.hpp>

namespace LunaraEngine
{

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

    void VulkanRendererAPI::HandleCommand(const RendererCommandType type) { HandleCommand(nullptr, type); }

    void VulkanRendererAPI::HandleCommand(const RendererCommand* command, const RendererCommandType type)
    {
        // clang-format off
        using CommandFunction = void (*)(RendererDataType*, const RendererCommand*);
        static const std::map<RendererCommandType, CommandFunction> opCommands = {
                {RendererCommandType::None,            (CommandFunction) VulkanRendererCommand::Nop},
                {RendererCommandType::BindShader,      (CommandFunction) VulkanRendererCommand::BindShader},
                {RendererCommandType::BindTexture,     (CommandFunction) VulkanRendererCommand::Nop},
                {RendererCommandType::Clear,           (CommandFunction) VulkanRendererCommand::Clear},
                {RendererCommandType::DrawQuad,        (CommandFunction) VulkanRendererCommand::Nop},
                {RendererCommandType::DrawTriangle,    (CommandFunction) VulkanRendererCommand::Nop},
                {RendererCommandType::DrawTexture,     (CommandFunction) VulkanRendererCommand::Nop},
                {RendererCommandType::DrawCircle,      (CommandFunction) VulkanRendererCommand::Nop},
                {RendererCommandType::DrawText,        (CommandFunction) VulkanRendererCommand::Nop},
                {RendererCommandType::DrawIndexed,     (CommandFunction) VulkanRendererCommand::DrawIndexed},
                {RendererCommandType::BeginRenderPass, (CommandFunction) VulkanRendererCommand::BeginRenderPass},
                {RendererCommandType::EndRenderPass,   (CommandFunction) VulkanRendererCommand::EndRenderPass},
                {RendererCommandType::Submit,          (CommandFunction) VulkanRendererCommand::Nop},
                {RendererCommandType::BeginFrame,      (CommandFunction) VulkanRendererCommand::BeginFrame},
                {RendererCommandType::Present,         (CommandFunction) VulkanRendererCommand::Present},
        };
        // clang-format on
        std::invoke(opCommands.at(type), m_RendererData.get(), command);
    }

    void VulkanRendererAPI::Present() { throw std::runtime_error("Not implemented"); }

    void VulkanRendererAPI::Init(const RendererAPIConfig& config)
    {
        m_Config = config;
        m_RendererData = std::make_shared<RendererDataType>();
        m_RendererData->surfaceExtent = {m_Config.initialWidth, m_Config.initialHeight};

        CreateWindow();
        VulkanInitializer::Initialize(m_RendererData.get());

        m_RendererData->swapChain =
                new SwapChain(m_RendererData->device, m_RendererData->physicalDevice, m_RendererData->vkSurface);
        m_RendererData->swapChain->Create(m_RendererData->surfaceExtent);

        m_RendererData->maxFramesInFlight = static_cast<uint32_t>(m_RendererData->swapChain->GetImages().size());

        m_RendererData->commandPool = new CommandPool(m_RendererData->device, m_RendererData->gfxQueue.GetIndex(),
                                                      m_RendererData->maxFramesInFlight);
        VulkanInitializer::CreateSyncObjects(m_RendererData.get());
    }

    void VulkanRendererAPI::Destroy()
    {
        vkDeviceWaitIdle(m_RendererData->device);
        delete m_RendererData->commandPool;
        delete m_RendererData->swapChain;
        VulkanInitializer::Goodbye(m_RendererData.get());
        SDL_DestroyWindow(static_cast<SDL_Window*>(m_RendererData->window->data));
    }

}// namespace LunaraEngine