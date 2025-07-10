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
#include <Renderer/Vulkan/VulkanRendererCommands.hpp>
#include <Renderer/Vulkan/Pipeline/Pipeline.hpp>
#include <Renderer/Vulkan/Buffer/IndexBuffer.hpp>
#include <Renderer/Vulkan/Buffer/VertexBuffer.hpp>
#include <Renderer/Vulkan/Buffer/StorageBuffer.hpp>
#include <Renderer/Vulkan/Shader.hpp>
#include <Renderer/Buffer/IndexBuffer.hpp>
#include <Renderer/Buffer/VertexBuffer.hpp>
#include <Renderer/Buffer/StorageBuffer.hpp>
#include <Renderer/Shader.hpp>

namespace LunaraEngine
{

    void VulkanRendererCommand::BindShader(RendererDataType* rendererData, const RendererCommand* command)
    {
        auto arg = static_cast<const RendererCommandBindShader*>(command);
        VulkanShader* shader = (VulkanShader*) (arg->shader);

        const auto& buffer = rendererData->commandPool->GetBuffer(rendererData->currentFrame);
        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipeline());
        if (arg->push_constants)
        {
            vkCmdPushConstants(buffer, shader->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, 128,
                               (void*) &arg->push_constants);
        }
        vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, 1,
                                &shader->GetDescriptorSets()[rendererData->currentFrame], 0, nullptr);
        shader->UpdateDescriptorSets(rendererData->currentFrame);
    }

    void VulkanRendererCommand::DrawQuad(RendererDataType* rendererData, const RendererCommand* command)
    {
        (void) rendererData;
        (void) command;

        throw std::runtime_error("Not implemented");
    }

    void VulkanRendererCommand::Clear(RendererDataType* rendererData, const RendererCommand* command)
    {
        rendererData->clearValue.color.float32[0] = static_cast<const RendererCommandClear*>(command)->r;
        rendererData->clearValue.color.float32[1] = static_cast<const RendererCommandClear*>(command)->g;
        rendererData->clearValue.color.float32[2] = static_cast<const RendererCommandClear*>(command)->b;
        rendererData->clearValue.color.float32[3] = static_cast<const RendererCommandClear*>(command)->a;
    }

    void VulkanRendererCommand::DrawIndexed(RendererDataType* rendererData, const RendererCommand* command)
    {
        auto arg = static_cast<const RendererCommandDrawIndexed*>(command);
        VulkanVertexBuffer* vertBuffer = (VulkanVertexBuffer*) (arg->vb->GetHandle());
        VulkanIndexBuffer* indexBuffer = (VulkanIndexBuffer*) (arg->ib->GetHandle());

        const auto& buffer = rendererData->commandPool->GetBuffer(rendererData->currentFrame);

        std::array<VkBuffer, 1> vertexBuffers = {vertBuffer->GetHandle()};
        std::array<VkDeviceSize, 1> offsets = {0};
        vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers.data(), offsets.data());
        vkCmdBindIndexBuffer(buffer, indexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT16);
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

    void VulkanRendererCommand::DrawInstanced(RendererDataType* rendererData, const RendererCommand* command)
    {
        auto arg = static_cast<const RendererCommandDrawInstanced*>(command);
        VulkanVertexBuffer* vertBuffer = (VulkanVertexBuffer*) (arg->vb->GetHandle());
        VulkanIndexBuffer* indexBuffer = (VulkanIndexBuffer*) (arg->ib->GetHandle());

        const auto& buffer = rendererData->commandPool->GetBuffer(rendererData->currentFrame);

        std::array<VkBuffer, 1> vertexBuffers = {vertBuffer->GetHandle()};
        std::array<VkDeviceSize, 1> offsets = {0};
        vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers.data(), offsets.data());
        vkCmdBindIndexBuffer(buffer, indexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT16);

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
        vkCmdDrawIndexed(buffer, static_cast<uint32_t>(indexBuffer->GetLength()), arg->count, 0, 0, 0);
    }

    void VulkanRendererCommand::DrawQuadBatch(RendererDataType* rendererData, const RendererCommand* command)
    {
        auto arg = static_cast<const RendererCommandDrawBatch*>(command);


        for (const auto& upload: arg->uploadList)
        {
            const auto& [storageBuffer, data] = upload;
            VulkanStorageBuffer* batchStorage = (VulkanStorageBuffer*) (storageBuffer);
            auto size = data.size();
            auto stride = batchStorage->GetStride();
            batchStorage->Upload(data.data(), size, stride);
        }

        const auto& buffer = rendererData->commandPool->GetBuffer(rendererData->currentFrame);

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

        vkCmdDraw(buffer, 6, (uint32_t) arg->count, 0, (uint32_t) arg->offset);
    }

    void VulkanRendererCommand::BeginRenderPass(RendererDataType* rendererData, const RendererCommand* command)
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

    void VulkanRendererCommand::EndRenderPass(RendererDataType* rendererData, const RendererCommand* command)
    {
        (void) command;
        const auto& buffer = rendererData->commandPool->GetBuffer(rendererData->currentFrame);
        vkCmdEndRenderPass(buffer);
        buffer.EndRecording();
    }

    void VulkanRendererCommand::BeginFrame(RendererDataType* rendererData, const RendererCommand* command)
    {
        (void) command;
        vkWaitForFences(
                rendererData->device, 1, &rendererData->inFlightFence[rendererData->currentFrame], VK_TRUE, UINT64_MAX);
        vkAcquireNextImageKHR(rendererData->device, rendererData->swapChain->GetSwapChain(), UINT64_MAX,
                              rendererData->imageAvailableSemaphore[rendererData->currentFrame], VK_NULL_HANDLE,
                              &(rendererData->imageIndex));

        vkResetFences(rendererData->device, 1, &(rendererData->inFlightFence[rendererData->currentFrame]));

        vkResetCommandBuffer(rendererData->commandPool->GetBuffer(rendererData->currentFrame),
                             /*VkCommandBufferResetFlagBits*/ 0);
    }

    void VulkanRendererCommand::Present(RendererDataType* rendererData, const RendererCommand* command)
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

    void VulkanRendererCommand::Nop(RendererDataType* rendererData, const RendererCommand* command)
    {
        (void) rendererData;
        (void) command;
    }

}// namespace LunaraEngine