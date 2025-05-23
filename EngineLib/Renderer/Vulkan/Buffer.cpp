#include "Buffer.hpp"
#include <stdexcept>
#include "Common.hpp"

namespace LunaraEngine
{
    Buffer::Buffer(VkDevice device) : m_Device(device) {}

    void Buffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }
    }

    void Buffer::BindBufferToDevMemory(VkDeviceMemory& bufferMemory, VkMemoryPropertyFlags properties)
    {
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(m_Device, m_Buffer, bufferMemory, 0);
    }
}// namespace LunaraEngine