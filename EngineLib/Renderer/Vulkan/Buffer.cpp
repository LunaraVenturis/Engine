#include "Buffer.hpp"
#include <stdexcept>
#include <Renderer/Vulkan/Common.hpp>
#include <Renderer/Vulkan/CommandPool.hpp>
#include <Renderer/Vulkan/Synchronization.hpp>
#include <cassert>

namespace LunaraEngine
{
    void Buffer::CreateBuffer(VkBufferUsageFlags usage)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = m_Size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto result = vkCreateBuffer(m_Device, &bufferInfo, nullptr, &m_Buffer);
        assert(result == VK_SUCCESS);
        (void) result;
    }

    void Buffer::BindBufferToDevMemory(VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
    {
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);

        auto result = vkAllocateMemory(m_Device, &allocInfo, nullptr, &m_BufferMemory);
        assert(result == VK_SUCCESS);
        (void) result;

        result = vkBindBufferMemory(m_Device, m_Buffer, m_BufferMemory, 0);
        assert(result == VK_SUCCESS);

        result = vkMapMemory(m_Device, m_BufferMemory, 0, m_Size, 0, (void**) &m_MappedDataPtr);
        assert(result == VK_SUCCESS);
    }

    void Buffer::Destroy()
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

    ShaderResourceType Buffer::GetResourceType() const { return m_ResourceType; }

    Buffer::~Buffer() { Destroy(); }

    void Buffer::Upload(size_t offset, uint8_t* data, size_t length, size_t stride)
    {
        assert(offset + length * stride <= m_Size);
        assert(m_MappedDataPtr != nullptr);
        uint8_t* newPtr = m_MappedDataPtr + offset;
        size_t size = length * stride;
        memcpy(newPtr, data, size);
    }

    void Buffer::Upload(uint8_t* data, size_t length, size_t stride) { Upload(0, data, length, stride); }

    void Buffer::CopyTo(CommandPool* commandPool, VkQueue executeQueue, Buffer* buffer)
    {
        VulkanFence fence(m_Device);
        auto cmdBuffer = commandPool->CreateImmediateCommandBuffer();
        cmdBuffer->BeginRecording();
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = m_Size;
        vkCmdCopyBuffer(*cmdBuffer, m_Buffer, buffer->GetBuffer(), 1, &copyRegion);

        cmdBuffer->EndRecording();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        VkCommandBuffer buffers[] = {*cmdBuffer};
        submitInfo.pCommandBuffers = buffers;
        vkQueueSubmit(executeQueue, 1, &submitInfo, fence);
        fence.Wait();
        fence.Destroy();
    }
}// namespace LunaraEngine