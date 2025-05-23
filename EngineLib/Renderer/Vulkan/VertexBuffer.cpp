#include "VertexBuffer.hpp"
#include <cstring>
namespace LunaraEngine
{
    VertexBuffer::VertexBuffer() : Buffer(m_Device), m_Size(sizeof(vertices[0]) * vertices.size())
    {
        CreateBuffer(m_Size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        BindBufferToDevMemory(m_VertexBufferMemory,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void* data;
        vkMapMemory(m_Device, m_VertexBufferMemory, 0, m_Size, 0, &data);
        memcpy(data, vertices.data(), (size_t) m_Size);
        vkUnmapMemory(m_Device, m_VertexBufferMemory);
    }

    VertexBuffer::VertexBuffer(VkDeviceSize size)
        : Buffer(m_Device), m_Size(sizeof(vertices[0]) * vertices.size())
    {
        CreateBuffer(m_Size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        BindBufferToDevMemory(m_VertexBufferMemory,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void* data;
        vkMapMemory(m_Device, m_VertexBufferMemory, 0, m_Size, 0, &data);
        memcpy(data, vertices.data(), (size_t) m_Size);
        vkUnmapMemory(m_Device, m_VertexBufferMemory);
    }

    void VertexBuffer::Upload(const VertexBuffer* buffer)
    {
        std::memcpy(this, buffer, sizeof(buffer));
    }
}// namespace LunaraEngine