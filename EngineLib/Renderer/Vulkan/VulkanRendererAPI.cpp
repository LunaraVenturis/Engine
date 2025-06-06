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
                {RendererCommandType::None,            (CommandFunction) VulkanRendererAPI::_NOP},
                {RendererCommandType::BindShader,      (CommandFunction) VulkanRendererAPI::_BindShader},
                {RendererCommandType::BindTexture,     (CommandFunction) VulkanRendererAPI::_NOP},
                {RendererCommandType::Clear,           (CommandFunction) VulkanRendererAPI::_Clear},
                {RendererCommandType::DrawQuad,        (CommandFunction) VulkanRendererAPI::_NOP},
                {RendererCommandType::DrawTriangle,    (CommandFunction) VulkanRendererAPI::_NOP},
                {RendererCommandType::DrawTexture,     (CommandFunction) VulkanRendererAPI::_NOP},
                {RendererCommandType::DrawCircle,      (CommandFunction) VulkanRendererAPI::_NOP},
                {RendererCommandType::DrawText,        (CommandFunction) VulkanRendererAPI::_NOP},
                {RendererCommandType::DrawIndexed,     (CommandFunction) VulkanRendererAPI::_DrawIndexed},
                {RendererCommandType::BeginRenderPass, (CommandFunction) VulkanRendererAPI::_BeginRenderPass},
                {RendererCommandType::EndRenderPass,   (CommandFunction) VulkanRendererAPI::_EndRenderPass},
                {RendererCommandType::Submit,          (CommandFunction) VulkanRendererAPI::_NOP},
                {RendererCommandType::BeginFrame,      (CommandFunction) VulkanRendererAPI::_BeginFrame},
                {RendererCommandType::Present,         (CommandFunction) VulkanRendererAPI::_Present},
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

    void VulkanRendererAPI::_BindShader(RendererDataType* rendererData, const RendererCommandBindShader* command)
    {
        auto arg = static_cast<const RendererCommandBindShader*>(command);
        VulkanShader* shader = (VulkanShader*) (arg->shader->GetHandle());

        const auto& buffer = rendererData->commandPool->GetBuffer(rendererData->currentFrame);
        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipeline());
        vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, 1,
                                &shader->GetDescriptorSets()[rendererData->currentFrame], 0, nullptr);
    }

    void VulkanRendererAPI::_DrawQuad(RendererDataType* rendererData, const RendererCommandDrawQuad* command)
    {
        (void) rendererData;
        (void) command;

        throw std::runtime_error("Not implemented");
    }

    void VulkanRendererAPI::_Clear(RendererDataType* rendererData, const RendererCommandClear* command)
    {
        rendererData->clearValue.color.float32[0] = static_cast<const RendererCommandClear*>(command)->r;
        rendererData->clearValue.color.float32[1] = static_cast<const RendererCommandClear*>(command)->g;
        rendererData->clearValue.color.float32[2] = static_cast<const RendererCommandClear*>(command)->b;
        rendererData->clearValue.color.float32[3] = static_cast<const RendererCommandClear*>(command)->a;
    }

    void VulkanRendererAPI::_DrawIndexed(RendererDataType* rendererData, const RendererCommandDrawIndexed* command)
    {
        auto arg = static_cast<const RendererCommandDrawIndexed*>(command);
        VulkanVertexBuffer* vertBuffer = (VulkanVertexBuffer*) (arg->vb->GetHandle());
        VulkanIndexBuffer* indexBuffer = (VulkanIndexBuffer*) (arg->ib->GetHandle());

        const auto& buffer = rendererData->commandPool->GetBuffer(rendererData->currentFrame);

        std::array<VkBuffer, 1> vertexBuffers = {vertBuffer->GetBuffer()};
        std::array<VkDeviceSize, 1> offsets = {0};
        vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers.data(), offsets.data());
        vkCmdBindIndexBuffer(buffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(rendererData->surfaceExtent.width);
        viewport.height = static_cast<float>(rendererData->surfaceExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewportWithCount(buffer, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = rendererData->surfaceExtent;
        vkCmdSetScissorWithCount(buffer, 1, &scissor);
        vkCmdDrawIndexed(buffer, static_cast<uint32_t>(indexBuffer->GetLength()), 1, 0, 0, 0);
    }

    void VulkanRendererAPI::_BeginRenderPass(RendererDataType* rendererData, const RendererCommand* command)
    {
        (void) command;
        const auto& buffer = rendererData->commandPool->GetBuffer(rendererData->currentFrame);
        buffer.BeginRecording();
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.framebuffer = rendererData->swapChain->GetFrameBuffer(rendererData->imageIndex);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = rendererData->surfaceExtent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &rendererData->clearValue;
        renderPassInfo.renderPass = rendererData->swapChain->GetRenderPass();
        vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanRendererAPI::_EndRenderPass(RendererDataType* rendererData, const RendererCommand* command)
    {
        (void) command;
        const auto& buffer = rendererData->commandPool->GetBuffer(rendererData->currentFrame);
        vkCmdEndRenderPass(buffer);
        buffer.EndRecording();
    }

    void VulkanRendererAPI::_BeginFrame(RendererDataType* rendererData, const RendererCommand* command)
    {
        (void) command;
        vkWaitForFences(rendererData->device, 1, &rendererData->inFlightFence[rendererData->currentFrame], VK_TRUE,
                        UINT64_MAX);
        vkAcquireNextImageKHR(rendererData->device, rendererData->swapChain->GetSwapChain(), UINT64_MAX,
                              rendererData->imageAvailableSemaphore[rendererData->currentFrame], VK_NULL_HANDLE,
                              &(rendererData->imageIndex));

        vkResetFences(rendererData->device, 1, &(rendererData->inFlightFence[rendererData->currentFrame]));

        vkResetCommandBuffer(rendererData->commandPool->GetBuffer(rendererData->currentFrame),
                             /*VkCommandBufferResetFlagBits*/ 0);
    }

    void VulkanRendererAPI::_Present(RendererDataType* rendererData, const RendererCommand* command)
    {
        (void) command;

        std::array<VkSemaphore, 1> waitSemaphores = {rendererData->imageAvailableSemaphore[rendererData->currentFrame]};
        std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        std::array<VkCommandBuffer, 1> buffer = {rendererData->commandPool->GetBuffer(rendererData->currentFrame)};
        std::array<VkSemaphore, 1> signalSemaphores = {
                rendererData->renderFinishedSemaphore[rendererData->currentFrame]};
        std::array<VkSwapchainKHR, 1> swapChains = {rendererData->swapChain->GetSwapChain()};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores.data();
        submitInfo.pWaitDstStageMask = waitStages.data();

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffer.data();

        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores.data();

        if (vkQueueSubmit(rendererData->gfxQueue, 1, &submitInfo,
                          rendererData->inFlightFence[rendererData->currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores.data();
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains.data();
        presentInfo.pImageIndices = &(rendererData->imageIndex);
        vkQueuePresentKHR(rendererData->presentQueue, &presentInfo);

        rendererData->currentFrame = (rendererData->currentFrame + 1) % rendererData->inFlightFence.size();
    }

    void VulkanRendererAPI::_NOP(RendererDataType* rendererData, const RendererCommand* command)
    {
        (void) rendererData;
        (void) command;
    }

}// namespace LunaraEngine