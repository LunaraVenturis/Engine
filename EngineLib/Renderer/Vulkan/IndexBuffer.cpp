#include "IndexBuffer.hpp"

namespace LunaraEngine
{
    IndexBuffer::IndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, CommandPool* commandPool, VkQueue executeQueue,
                     uint8_t* data, size_t size)
    {
        Create(device, physicalDevice, commandPool, executeQueue, data, size);
    }
    void IndexBuffer::Create(VkDevice device, VkPhysicalDevice physicalDevice, CommandPool* commandPool,
                              VkQueue executeQueue, uint8_t* data, size_t size)
    {
        m_Device = device;
        m_Size = size;

        m_StagingBuffer.Create(device, physicalDevice, data, size);

        CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        BindBufferToDevMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, physicalDevice);

        m_StagingBuffer.CopyTo(commandPool, executeQueue, this);

        m_StagingBuffer.Destroy();
    }

}