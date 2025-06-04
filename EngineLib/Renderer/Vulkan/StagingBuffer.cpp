#include "StagingBuffer.hpp"
#include <cstring>

namespace LunaraEngine
{
    StagingBuffer::StagingBuffer(VkDevice device, VkPhysicalDevice physicalDevice, uint8_t* data, size_t length,
                                 size_t stride)
    {
        Create(device, physicalDevice, data, length, stride);
    }

    void StagingBuffer::Create(VkDevice device, VkPhysicalDevice physicalDevice, uint8_t* data, size_t length,
                               size_t stride)
    {
        m_Size = length * stride;
        m_Device = device;
        m_Stride = stride;
        CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        BindBufferToDevMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              physicalDevice);
        if (data != nullptr) { Upload(data, length, stride); }
    }

}// namespace LunaraEngine