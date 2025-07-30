#include "Buffer.hpp"
#include <stdexcept>
#include <LunaraEngine/Renderer/Vulkan/Common.hpp>
#include <LunaraEngine/Renderer/Vulkan/CommandPool.hpp>
#include <LunaraEngine/Renderer/Vulkan/Synchronization.hpp>
#include <cassert>
#include <cstring>

namespace LunaraEngine
{

    template <BufferResourceType type>
    void Buffer<type>::BindBufferToDevMemory(VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
    {
        VkMemoryRequirements memRequirements;
        switch (m_ResourceType)
        {
            case BufferResourceType::Texture:
                vkGetImageMemoryRequirements(m_Device, m_Image, &memRequirements);
                break;
            default:
                vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memRequirements);
                break;
        }

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);

        auto result = vkAllocateMemory(m_Device, &allocInfo, nullptr, &m_BufferMemory);
        assert(result == VK_SUCCESS);
        (void) result;

        switch (m_ResourceType)
        {
            case BufferResourceType::Texture:
                result = vkBindImageMemory(m_Device, m_Image, m_BufferMemory, 0);
                break;
            default:
                result = vkBindBufferMemory(m_Device, m_Buffer, m_BufferMemory, 0);
                assert(result == VK_SUCCESS);

                result = vkMapMemory(m_Device, m_BufferMemory, 0, m_Size * m_Stride, 0, (void**) &m_MappedDataPtr);
                assert(result == VK_SUCCESS);
                break;
        }
    }

    template <BufferResourceType type>
    void Buffer<type>::TransitionLayout(CommandBuffer* cmdBuffer, VkFormat format, VkImageLayout oldLayout,
                                        VkImageLayout newLayout, uint32_t baseLayer) const
            requires(type == BufferResourceType::Texture)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_Image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = baseLayer;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else { throw std::invalid_argument("unsupported layout transition!"); }

        vkCmdPipelineBarrier(*cmdBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        (void) format;
    }

    template <BufferResourceType type>
    void Buffer<type>::Destroy()
    {
        if (m_BufferMemory != VK_NULL_HANDLE)
        {
            auto result = vkDeviceWaitIdle(m_Device);
            assert(result == VK_SUCCESS);
            (void) result;

            if (m_MappedDataPtr != nullptr) { vkUnmapMemory(m_Device, m_BufferMemory); }

            if (m_Buffer != VK_NULL_HANDLE) { vkDestroyBuffer(m_Device, m_Buffer, nullptr); }
            vkFreeMemory(m_Device, m_BufferMemory, nullptr);
            m_Device = VK_NULL_HANDLE;
            m_Buffer = VK_NULL_HANDLE;
            m_BufferMemory = VK_NULL_HANDLE;
            m_Size = 0;
        }
    }

    template <BufferResourceType type>
    BufferResourceType Buffer<type>::GetResourceType() const
    {
        return m_ResourceType;
    }

    template <BufferResourceType type>
    Buffer<type>::~Buffer()
    {
        Destroy();
    }

    template <BufferResourceType type>
    void Buffer<type>::Upload(size_t offset, uint8_t* data, size_t length, size_t stride)
    {
        assert(offset + length * stride <= m_Size * m_Stride);
        assert(m_MappedDataPtr != nullptr);
        uint8_t* newPtr = m_MappedDataPtr + offset;
        size_t size = length * stride;
        std::memcpy(newPtr, data, size);
    }

    template <BufferResourceType type>
    void Buffer<type>::Upload(uint8_t* data, size_t length, size_t stride)
    {
        Upload(0, data, length, stride);
    }

    template <BufferResourceType type>
    std::shared_ptr<CommandBuffer> Buffer<type>::BeginRecording(CommandPool* commandPool)
    {
        auto cmdBuffer = commandPool->CreateImmediateCommandBuffer();
        cmdBuffer->BeginRecording();
        return cmdBuffer;
    }

    template <BufferResourceType type>
    void Buffer<type>::Submit(CommandBuffer* cmdBuffer, VkQueue executeQueue, VulkanFence* fence)
    {
        cmdBuffer->EndRecording();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        VkCommandBuffer buffers[] = {*cmdBuffer};
        submitInfo.pCommandBuffers = buffers;
        vkQueueSubmit(executeQueue, 1, &submitInfo, *fence);
        fence->Wait();
    }

    template <BufferResourceType type>
    template <BufferResourceType U>
    void Buffer<type>::CopyTo(CommandPool* commandPool, VkQueue executeQueue, Buffer<U>* buffer)
    {
        VulkanFence fence(m_Device);
        auto cmdBuffer = commandPool->CreateImmediateCommandBuffer();
        cmdBuffer->BeginRecording();

        if constexpr (type == BufferResourceType::Texture && U == BufferResourceType::Buffer)
        {
            VkBufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;

            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;

            region.imageOffset = {0, 0, 0};
            region.imageExtent = {buffer->m_Dimensions.width, buffer->m_Dimensions.height, 1};
            vkCmdCopyBufferToImage(
                    *cmdBuffer, m_Buffer, buffer->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        }
        else
        {
            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = m_Size;
            vkCmdCopyBuffer(*cmdBuffer, m_Buffer, buffer->GetHandle(), 1, &copyRegion);
        }
        Submit(cmdBuffer.get(), executeQueue, &fence);
        fence.Destroy();
    }

    template <BufferResourceType type>
    template <BufferResourceType U>
    void Buffer<type>::CopyTo(CommandBuffer* cmdBuffer, Buffer<U>* buffer, VkDeviceSize offset, uint32_t baseLayer)
    {
        if constexpr (type == BufferResourceType::Buffer && U == BufferResourceType::Texture)
        {
            VkBufferImageCopy region{};
            region.bufferOffset = offset;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;

            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = baseLayer;
            region.imageSubresource.layerCount = 1;

            region.imageOffset = {0, 0, 0};
            region.imageExtent = VkExtent3D{(uint32_t) buffer->GetWidth(), (uint32_t) buffer->GetHeight(), 1};
            vkCmdCopyBufferToImage(
                    *cmdBuffer, m_Buffer, buffer->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        }
        else
        {
            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = m_Size;
            vkCmdCopyBuffer(*cmdBuffer, m_Buffer, buffer->GetHandle(), 1, &copyRegion);
        }
    }
}// namespace LunaraEngine