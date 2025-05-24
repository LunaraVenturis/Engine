#include "StagingBuffer.hpp"
#include <cstring>

namespace LunaraEngine
{
    StagingBuffer::StagingBuffer(VkDevice device) : Buffer(device), m_Device(device) {}

    StagingBuffer::StagingBuffer(VkDevice device, VkPhysicalDevice physicalDevice)
        : Buffer(device), m_Device(device),
          m_Size((sizeof(VertexBuffer::GetVertices()[0]) * VertexBuffer::GetVertices().size()))
    {
        CreateBuffer(m_Size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        BindBufferToDevMemory(m_StagingBufferMemory,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              physicalDevice);
        void* data;
        vkMapMemory(m_Device, m_StagingBufferMemory, 0, m_Size, 0, &data);
        std::memcpy(data, VertexBuffer::GetVertices().data(), (size_t) m_Size);
        vkUnmapMemory(m_Device, m_StagingBufferMemory);
    }

    void StagingBuffer::Upload(VkPhysicalDevice physicalDevice)
    {
        CreateBuffer(m_Size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        BindBufferToDevMemory(m_StagingBufferMemory,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              physicalDevice);
        void* data;
        vkMapMemory(m_Device, m_StagingBufferMemory, 0, m_Size, 0, &data);
        std::memcpy(data, VertexBuffer::GetVertices().data(), (size_t) m_Size);
        vkUnmapMemory(m_Device, m_StagingBufferMemory);
    }
}// namespace LunaraEngine