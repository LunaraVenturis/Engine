#include "StorageBuffer.hpp"
#include "CommandPool.hpp"
#include "VulkanDataTypes.hpp"
#include <cstring>

namespace LunaraEngine
{
    VulkanStorageBuffer::VulkanStorageBuffer(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data,
                                             size_t length, size_t stride)
    {
        Create(rendererData, executeQueue, data, length, stride);
    }

    void VulkanStorageBuffer::Create(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                                     size_t stride)
    {
        m_ResourceType = ShaderResourceType::StorageBuffer;
        m_Device = rendererData->device;
        m_Size = length * stride;
        m_Stride = stride;

        // m_StagingBuffer.Create(m_Device, rendererData->physicalDevice, nullptr, length, stride);
        CreateBuffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
        BindBufferToDevMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              rendererData->physicalDevice);

        Upload(rendererData, executeQueue, data, length, stride);
    }

    void VulkanStorageBuffer::Upload(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                                     size_t stride)
    {
        if (data == nullptr) { return; }
        if (length * stride > m_Size || length == 0) { return; }
        if (!m_StagingBuffer.IsValid())
        {
            // m_StagingBuffer.Create(m_Device, rendererData->physicalDevice, nullptr, length, stride);
        }

        (void) rendererData;
        (void) executeQueue;

        uint8_t* m_Data;
        vkMapMemory(m_Device, m_BufferMemory, 0, m_Size, 0, (void**) &m_Data);
        memcpy(m_Data, data, length * stride);
        vkUnmapMemory(m_Device, m_BufferMemory);
        // m_StagingBuffer.Upload(data, length, stride);
        // m_StagingBuffer.CopyTo(rendererData->commandPool, executeQueue, this);
        // m_StagingBuffer.Destroy();
    }

}// namespace LunaraEngine