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
 * 
 */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL.h>
#include <LunaraEngine/Core/Log.h>
#include <LunaraEngine/Renderer/Vulkan/VulkanRendererAPI.hpp>
#include <LunaraEngine/Renderer/Window.hpp>
#include <LunaraEngine/Renderer/Vulkan/VulkanInitialization.hpp>
#include <LunaraEngine/Renderer/Vulkan/Buffer/IndexBuffer.hpp>
#include <LunaraEngine/Renderer/Vulkan/Buffer/VertexBuffer.hpp>
#include <LunaraEngine/Renderer/Vulkan/Shader.hpp>
#include <LunaraEngine/Renderer/Vulkan/VulkanRendererCommands.hpp>
#include <LunaraEngine/Renderer/Buffer/IndexBuffer.hpp>
#include <LunaraEngine/Renderer/Buffer/VertexBuffer.hpp>
#include <LunaraEngine/Renderer/Shader.hpp>
#include "VulkanRendererAPI.hpp"

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

        SDL_SetWindowPosition(
                static_cast<SDL_Window*>(m_RendererData->window->data), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    constexpr auto VulkanRendererAPI::MakeDispatchableTable()
    {
        using DispatchFunction = void (*)(RendererDataType*, const RendererCommand*);

        auto CastToDispatchable = [](auto f) { return static_cast<DispatchFunction>(f); };
        auto CastToIndex = [](RendererCommandType type) { return static_cast<size_t>(type); };

        // clang-format off
        constexpr auto registeredCommands =
                std::array<std::tuple<RendererCommandType, DispatchFunction>, CastToIndex(RendererCommandType::Count)>{
                        std::tuple<RendererCommandType, DispatchFunction>{RendererCommandType::None, VulkanRendererCommand::Nop},
                        {RendererCommandType::BindShader, VulkanRendererCommand::BindShader},
                        {RendererCommandType::BindTexture, VulkanRendererCommand::Nop},
                        {RendererCommandType::Clear, VulkanRendererCommand::Clear},
                        {RendererCommandType::DrawQuad, VulkanRendererCommand::Nop},
                        {RendererCommandType::DrawTriangle, VulkanRendererCommand::Nop},
                        {RendererCommandType::DrawTexture, VulkanRendererCommand::Nop},
                        {RendererCommandType::DrawCircle, VulkanRendererCommand::Nop},
                        {RendererCommandType::DrawText, VulkanRendererCommand::Nop},
                        {RendererCommandType::DrawIndexed, VulkanRendererCommand::DrawIndexed},
                        {RendererCommandType::DrawInstanced, VulkanRendererCommand::DrawInstanced},
                        {RendererCommandType::BeginRenderPass, VulkanRendererCommand::BeginRenderPass},
                        {RendererCommandType::EndRenderPass, VulkanRendererCommand::EndRenderPass},
                        {RendererCommandType::Submit, VulkanRendererCommand::Nop},
                        {RendererCommandType::BeginFrame, VulkanRendererCommand::BeginFrame},
                        {RendererCommandType::Present, VulkanRendererCommand::Present},
                        {RendererCommandType::DrawQuadBatch, VulkanRendererCommand::DrawQuadBatch},
                    };

        std::array<DispatchFunction, static_cast<size_t>(RendererCommandType::Count)> table;
        for (auto& command: registeredCommands)
        {
            table[CastToIndex(std::get<RendererCommandType>(command))] = CastToDispatchable(std::get<DispatchFunction>(command));
        }
        // clang-format on

        return table;
    }

    Window* VulkanRendererAPI::GetWindow() { return m_RendererData->window; }

    void VulkanRendererAPI::HandleCommand(const RendererCommandType type) { HandleCommand(nullptr, type); }

    size_t VulkanRendererAPI::GetWidth() const { return m_RendererData->surfaceExtent.width; }

    size_t VulkanRendererAPI::GetHeight() const { return m_RendererData->surfaceExtent.height; }

    void VulkanRendererAPI::HandleCommand(const RendererCommand* command, const RendererCommandType type)
    {
        static constexpr auto dispatchTable = MakeDispatchableTable();
        const auto index = static_cast<size_t>(type);

        assert(index < static_cast<size_t>(RendererCommandType::Count));
        assert(dispatchTable[index] != nullptr);

        std::invoke(dispatchTable[index], m_RendererData.get(), command);
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

        m_RendererData->commandPool = new CommandPool(
                m_RendererData->device, m_RendererData->gfxQueue.GetIndex(), m_RendererData->maxFramesInFlight);
        VulkanInitializer::CreateSyncObjects(m_RendererData.get());
    }

    void VulkanRendererAPI::Destroy()
    {
        vkDeviceWaitIdle(m_RendererData->device);
        delete m_RendererData->commandPool;
        delete m_RendererData->swapChain;
        VulkanInitializer::Goodbye(m_RendererData.get());
        SDL_DestroyWindow(static_cast<SDL_Window*>(m_RendererData->window->data));
        delete m_RendererData->window;
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_Quit();
    }

}// namespace LunaraEngine