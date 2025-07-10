#include "IndexBuffer.hpp"
#include <Renderer/Vulkan/VulkanDataTypes.hpp>

namespace LunaraEngine
{
    VulkanIndexBuffer::VulkanIndexBuffer(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data,
                                         size_t length, size_t stride)
    {
        Create(rendererData, executeQueue, data, length, stride);
    }

    void VulkanIndexBuffer::Create(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                                   size_t stride)
    {
        m_Device = rendererData->device;
        m_Size = length * stride;
        m_Stride = stride;

        m_StagingBuffer.Create(rendererData->device, rendererData->physicalDevice, data, length, stride);

        CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        BindBufferToDevMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, rendererData->physicalDevice);

        m_StagingBuffer.CopyTo(rendererData->commandPool, executeQueue, this);

        m_StagingBuffer.Destroy();
    }

    void VulkanIndexBuffer::Upload(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                                   size_t stride)
    {
        if (data == nullptr) { return; }
        if (stride * length > m_Size || length == 0) { return; }
        if (!m_StagingBuffer.IsValid()) { return; }
        m_StagingBuffer.Upload(data, length, stride);
        m_StagingBuffer.CopyTo(rendererData->commandPool, executeQueue, this);
        m_StagingBuffer.Destroy();
    }

}// namespace LunaraEngine