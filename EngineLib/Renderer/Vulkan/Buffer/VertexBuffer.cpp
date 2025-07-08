#include "VertexBuffer.hpp"
#include <Renderer/BatchRenderer.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <cstring>

namespace LunaraEngine
{
    VulkanVertexBuffer::VulkanVertexBuffer(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data,
                                           size_t length, size_t stride)
    {
        Create(rendererData, executeQueue, data, length, stride);
    }

    void VulkanVertexBuffer::Create(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                                    size_t stride)
    {
        m_Device = rendererData->device;
        m_Size = length * stride;
        m_Stride = stride;

        m_StagingBuffer.Create(m_Device, rendererData->physicalDevice, nullptr, length, stride);

        CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        BindBufferToDevMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, rendererData->physicalDevice);

        Upload(rendererData, executeQueue, data, length, stride);
    }

    void VulkanVertexBuffer::Upload(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                                    size_t stride)
    {
        if (data == nullptr) { return; }
        if (length * stride > m_Size || length == 0) { return; }
        if (!m_StagingBuffer.IsValid()) { return; }

        m_StagingBuffer.Upload(data, length, stride);
        m_StagingBuffer.CopyTo(rendererData->commandPool, executeQueue, this);
        m_StagingBuffer.Destroy();
    }

}// namespace LunaraEngine