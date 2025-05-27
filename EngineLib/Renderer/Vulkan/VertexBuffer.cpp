#include "VertexBuffer.hpp"
#include "CommandPool.hpp"
#include <cstring>

namespace LunaraEngine
{
    VertexBuffer::VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, CommandPool* commandPool,
                               VkQueue executeQueue, uint8_t* data, size_t size)
    {
        Create(device, physicalDevice, commandPool, executeQueue, data, size);
    }

    void VertexBuffer::Create(VkDevice device, VkPhysicalDevice physicalDevice, CommandPool* commandPool,
                              VkQueue executeQueue, uint8_t* data, size_t size)
    {
        m_Device = device;
        m_Size = size;

        m_StagingBuffer.Create(device, physicalDevice, data, size);

        CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        BindBufferToDevMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, physicalDevice);

        m_StagingBuffer.CopyTo(commandPool, executeQueue, this);

        m_StagingBuffer.Destroy();
    }

}// namespace LunaraEngine