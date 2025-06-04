#include <Renderer/Vulkan/UniformBuffer.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>

namespace LunaraEngine
{
    VulkanUniformBuffer::VulkanUniformBuffer(RendererDataType* rendererData, uint8_t* data, size_t length,
                                             size_t stride)
    {
        Create(rendererData, data, length, stride);
    }

    void VulkanUniformBuffer::Create(RendererDataType* rendererData, uint8_t* data, size_t length, size_t stride)
    {
        m_Device = rendererData->device;
        m_Size = length * stride;
        m_Stride = stride;

        CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        BindBufferToDevMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              rendererData->physicalDevice);

        if (data == nullptr) { return; }
        uint8_t* m_Data;
        vkMapMemory(m_Device, m_BufferMemory, 0, m_Size, 0, (void**) &m_Data);
        memcpy(m_Data, data, length * stride);
        vkUnmapMemory(m_Device, m_BufferMemory);
    }

    void VulkanUniformBuffer::Upload(RendererDataType* rendererData, uint8_t* data, size_t length, size_t stride)
    {
        if (data == nullptr) { return; }
        if (stride * length > m_Size || length == 0) { return; }

        uint8_t* m_Data;
        vkMapMemory(rendererData->device, m_BufferMemory, 0, m_Size, 0, (void**) &m_Data);
        memcpy(rendererData->device, data, length * stride);
        vkUnmapMemory(rendererData->device, m_BufferMemory);
    }

    void VulkanUniformBuffer::Upload(RendererDataType* rendererData, size_t offset, uint8_t* data, size_t size)
    {
        if (data == nullptr) { return; }
        if (offset + size > m_Size || size == 0) { return; }

        uint8_t* m_Data;
        vkMapMemory(rendererData->device, m_BufferMemory, offset, size, 0, (void**) &m_Data);
        memcpy(rendererData->device, data, size);
        vkUnmapMemory(rendererData->device, m_BufferMemory);
    }

}// namespace LunaraEngine