#include <Renderer/Vulkan/Buffer/UniformBuffer.hpp>
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
        m_ResourceType = BufferResourceType::UniformBuffer;
        m_Device = rendererData->device;
        m_Size = length;
        m_Stride = stride;

        CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        BindBufferToDevMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              rendererData->physicalDevice);
        if (data != nullptr) { Upload(data, length, stride); }
    }

}// namespace LunaraEngine