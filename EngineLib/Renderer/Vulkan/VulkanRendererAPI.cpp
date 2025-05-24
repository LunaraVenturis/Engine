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
#include <vulkan/vulkan.h>
#include "SDL3/SDL_vulkan.h"
#include "SDL3/SDL.h"
#include "GraphicsPipeline.hpp"
#include <stdexcept>

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

    void VulkanRendererAPI::HandleCommand(const RendererCommandType type) { HandleCommand(nullptr, type); }

    void VulkanRendererAPI::HandleCommand(const RendererCommand* command, const RendererCommandType type)
    {
        switch (type)
        {
            case RendererCommandType::BeginRenderPass: {
                const auto& buffer = m_RendererData->commandPool->GetBuffer(m_RendererData->currentFrame);
                buffer.BeginRecording();
                VkRenderPassBeginInfo renderPassInfo = {};
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassInfo.framebuffer = m_RendererData->swapChain->GetFrameBuffer(m_RendererData->imageIndex);
                renderPassInfo.renderArea.offset = {0, 0};
                renderPassInfo.renderArea.extent = m_RendererData->surfaceExtent;
                renderPassInfo.clearValueCount = 1;
                renderPassInfo.pClearValues = &m_RendererData->clearValue;
                renderPassInfo.renderPass = m_RendererData->swapChain->GetRenderPass();
                vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
                break;
            }
            case RendererCommandType::EndRenderPass: {
                const auto& buffer = m_RendererData->commandPool->GetBuffer(m_RendererData->currentFrame);
                vkCmdEndRenderPass(buffer);
                buffer.EndRecording();
                break;
            }
            case RendererCommandType::Clear: {
                m_RendererData->clearValue.color.float32[0] = static_cast<const RendererCommandClear*>(command)->r;
                m_RendererData->clearValue.color.float32[1] = static_cast<const RendererCommandClear*>(command)->g;
                m_RendererData->clearValue.color.float32[2] = static_cast<const RendererCommandClear*>(command)->b;
                m_RendererData->clearValue.color.float32[3] = static_cast<const RendererCommandClear*>(command)->a;
                break;
            }
            case RendererCommandType::DrawTriangle: {
                const auto& buffer = m_RendererData->commandPool->GetBuffer(m_RendererData->currentFrame);
                vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_RendererData->pipeline->GetPipeline());
                std::array<VkBuffer, 1> vertexBuffers = {m_RendererData->vertexBuffer->GetBuffer()};
                std::array<VkDeviceSize, 1> offsets = {0};
                vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers.data(), offsets.data());
                VkViewport viewport{};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = static_cast<float>(m_RendererData->surfaceExtent.width);
                viewport.height = static_cast<float>(m_RendererData->surfaceExtent.height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                vkCmdSetViewport(buffer, 0, 1, &viewport);

                VkRect2D scissor{};
                scissor.offset = {0, 0};
                scissor.extent = m_RendererData->surfaceExtent;
                vkCmdSetScissor(buffer, 0, 1, &scissor);
                vkCmdDraw(buffer, static_cast<uint32_t>(m_RendererData->vertexBuffer->GetVertices().size()), 1, 0, 0);
                break;
            }

            case RendererCommandType::BeginFrame: {

                vkWaitForFences(m_RendererData->device, 1, &m_RendererData->inFlightFence[m_RendererData->currentFrame],
                                VK_TRUE, UINT64_MAX);
                vkAcquireNextImageKHR(m_RendererData->device, m_RendererData->swapChain->GetSwapChain(), UINT64_MAX,
                                      m_RendererData->imageAvailableSemaphore[m_RendererData->currentFrame],
                                      VK_NULL_HANDLE, &(m_RendererData->imageIndex));

                vkResetFences(m_RendererData->device, 1,
                              &(m_RendererData->inFlightFence[m_RendererData->currentFrame]));

                vkResetCommandBuffer(m_RendererData->commandPool->GetBuffer(m_RendererData->currentFrame),
                                     /*VkCommandBufferResetFlagBits*/ 0);
                break;
            }
            case RendererCommandType::Present: {

                std::array<VkSemaphore, 1> waitSemaphores = {
                        m_RendererData->imageAvailableSemaphore[m_RendererData->currentFrame]};
                std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
                std::array<VkCommandBuffer, 1> buffer = {
                        m_RendererData->commandPool->GetBuffer(m_RendererData->currentFrame)};
                std::array<VkSemaphore, 1> signalSemaphores = {
                        m_RendererData->renderFinishedSemaphore[m_RendererData->currentFrame]};
                std::array<VkSwapchainKHR, 1> swapChains = {m_RendererData->swapChain->GetSwapChain()};

                VkSubmitInfo submitInfo{};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.waitSemaphoreCount = 1;
                submitInfo.pWaitSemaphores = waitSemaphores.data();
                submitInfo.pWaitDstStageMask = waitStages.data();

                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = buffer.data();

                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = signalSemaphores.data();

                if (vkQueueSubmit(m_RendererData->gfxQueue, 1, &submitInfo,
                                  m_RendererData->inFlightFence[m_RendererData->currentFrame]) != VK_SUCCESS)
                {
                    throw std::runtime_error("failed to submit draw command buffer!");
                }

                VkPresentInfoKHR presentInfo{};
                presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

                presentInfo.waitSemaphoreCount = 1;
                presentInfo.pWaitSemaphores = signalSemaphores.data();
                presentInfo.swapchainCount = 1;
                presentInfo.pSwapchains = swapChains.data();
                presentInfo.pImageIndices = &(m_RendererData->imageIndex);
                vkQueuePresentKHR(m_RendererData->presentQueue, &presentInfo);

                m_RendererData->currentFrame =
                        (m_RendererData->currentFrame + 1) % m_RendererData->inFlightFence.size();
                break;
            }
            default:
                break;
        }
    }

    void VulkanRendererAPI::Present() { throw std::runtime_error("Not implemented"); }

    void VulkanRendererAPI::Init(const RendererAPIConfig& config)
    {
        m_Config = config;
        m_RendererData = std::make_unique<RendererDataType>();
        m_RendererData->surfaceExtent = {m_Config.initialWidth, m_Config.initialHeight};

        CreateWindow();
        VulkanInitializer::Initialize(m_RendererData.get());

        auto vertShaderCode = ReadFile(config.shadersDirectory / "output/vert.spv");
        auto fragShaderCode = ReadFile(config.shadersDirectory / "output/frag.spv");
        std::vector<VkDescriptorSetLayout> descriptorSets;

        m_RendererData->swapChain =
                new SwapChain(m_RendererData->device, m_RendererData->physicalDevice, m_RendererData->vkSurface);
        m_RendererData->swapChain->Create(m_RendererData->surfaceExtent);

        m_RendererData->pipeline = new GraphicsPipeline(m_RendererData->device, m_RendererData->swapChain,
                                                        vertShaderCode, fragShaderCode, descriptorSets);

        m_RendererData->maxFramesInFlight = static_cast<uint32_t>(m_RendererData->swapChain->GetImages().size());

        m_RendererData->commandPool = new CommandPool(m_RendererData->device, m_RendererData->gfxQueue.GetIndex(),
                                                      m_RendererData->maxFramesInFlight);
        m_RendererData->vertexBuffer = new VertexBuffer(m_RendererData->device);
        m_RendererData->vertexBuffer->Upload(m_RendererData->physicalDevice);

        VulkanInitializer::CreateSyncObjects(m_RendererData.get());
    }

    void VulkanRendererAPI::Destroy()
    {
        vkDeviceWaitIdle(m_RendererData->device);
        delete m_RendererData->vertexBuffer;
        delete m_RendererData->commandPool;
        delete m_RendererData->pipeline;
        delete m_RendererData->swapChain;
        VulkanInitializer::Goodbye(m_RendererData.get());
        SDL_DestroyWindow(static_cast<SDL_Window*>(m_RendererData->window->data));
    }


}// namespace LunaraEngine