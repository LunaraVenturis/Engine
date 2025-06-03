#include "Buffer.hpp"
#include <stdexcept>
#include <Renderer/Vulkan/Common.hpp>
#include <Renderer/Vulkan/CommandPool.hpp>

namespace LunaraEngine
{
    void Buffer::CreateBuffer(VkBufferUsageFlags usage)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = m_Size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }
    }

    void Buffer::BindBufferToDevMemory(VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
    {
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);

        if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(m_Device, m_Buffer, m_BufferMemory, 0);
    }

    void Buffer::Destroy()
    {
        if (m_BufferMemory != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(m_Device);
            if (m_Buffer != VK_NULL_HANDLE) { vkDestroyBuffer(m_Device, m_Buffer, nullptr); }
            vkFreeMemory(m_Device, m_BufferMemory, nullptr);
            m_Device = VK_NULL_HANDLE;
            m_Buffer = VK_NULL_HANDLE;
            m_BufferMemory = VK_NULL_HANDLE;
            m_Size = 0;
        }
    }

    Buffer::~Buffer() { Destroy(); }

    void Buffer::Upload(uint8_t* data, size_t length, size_t stride)
    {
        m_Size = length * stride;
        m_Stride = stride;
        void* mappedMemory;
        vkMapMemory(m_Device, m_BufferMemory, 0, m_Size, 0, &mappedMemory);
        memcpy(mappedMemory, data, m_Size);
        vkUnmapMemory(m_Device, m_BufferMemory);
    }

    void Buffer::CopyTo(CommandPool* commandPool, VkQueue executeQueue, Buffer* buffer)
    {
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
        vkQueueSubmit(executeQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(executeQueue);
    }
}// namespace LunaraEngine