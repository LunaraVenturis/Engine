#include "VertexBuffer.hpp"
#include <cstring>

namespace LunaraEngine
{
    VertexBuffer::VertexBuffer(VkDevice device)
        : Buffer(device), m_Device(device), m_Size(sizeof(m_Vertices[0]) * m_Vertices.size())
    {}

    VertexBuffer::VertexBuffer(VkDevice device, VkDeviceSize size, VkPhysicalDevice physicalDevice) : Buffer(device), m_Device(device), m_Size(size)
    {
        CreateBuffer(m_Size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        BindBufferToDevMemory(m_VertexBufferMemory,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, physicalDevice);
        void* data;
        vkMapMemory(m_Device, m_VertexBufferMemory, 0, m_Size, 0, &data);
        std::memcpy(data, m_Vertices.data(), (size_t) m_Size);
        vkUnmapMemory(m_Device, m_VertexBufferMemory);
    }

    void VertexBuffer::Upload(VkPhysicalDevice physicalDevice)
    {
        CreateBuffer(m_Size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        BindBufferToDevMemory(m_VertexBufferMemory,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, physicalDevice);
        void* data;
        vkMapMemory(m_Device, m_VertexBufferMemory, 0, m_Size, 0, &data);
        std::memcpy(data, m_Vertices.data(), (size_t) m_Size);
        vkUnmapMemory(m_Device, m_VertexBufferMemory);
    }

    VertexBuffer::~VertexBuffer() { vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr); }
}// namespace LunaraEngine