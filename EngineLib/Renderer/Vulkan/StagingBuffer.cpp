#include "StagingBuffer.hpp"
#include <cstring>

namespace LunaraEngine
{
    StagingBuffer::StagingBuffer(VkDevice device, VkPhysicalDevice physicalDevice, uint8_t* data, size_t size)
    {
        Create(device, physicalDevice, data, size);
    }

    void StagingBuffer::Create(VkDevice device, VkPhysicalDevice physicalDevice, uint8_t* data, size_t size)
    {
        m_Size = size;
        m_Device = device;

        CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        BindBufferToDevMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              physicalDevice);

        Upload(data, size);
    }

}// namespace LunaraEngine