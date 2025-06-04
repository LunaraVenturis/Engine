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

        vkMapMemory(m_Device, m_BufferMemory, 0, m_Size, 0, (void**) &m_Data);

        if (data != nullptr) { memcpy(m_Data, data, length * stride); }
    }

    void VulkanUniformBuffer::Upload(RendererDataType* rendererData, uint8_t* data, size_t length, size_t stride)
    {
        if (data == nullptr) { return; }
        if (stride * length > m_Size || length == 0) { return; }

        if (m_Data == nullptr) { vkMapMemory(rendererData->device, m_BufferMemory, 0, m_Size, 0, (void**) &m_Data); }
        if (m_Data == nullptr) { throw std::runtime_error("Failed to map memory"); }
        memcpy(m_Data, data, length * stride);
    }

    void VulkanUniformBuffer::Upload(RendererDataType* rendererData, size_t offset, uint8_t* data, size_t size)
    {
        if (data == nullptr) { return; }
        if (offset + size > m_Size || size == 0) { return; }

        if (m_Data == nullptr) { vkMapMemory(rendererData->device, m_BufferMemory, 0, m_Size, 0, (void**) &m_Data); }
        if (m_Data == nullptr) { throw std::runtime_error("Failed to map memory"); }

        memcpy(m_Data + offset, data, size);
    }

}// namespace LunaraEngine