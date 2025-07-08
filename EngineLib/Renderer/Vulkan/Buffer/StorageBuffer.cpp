#include "StorageBuffer.hpp"
#include <Renderer/Vulkan/CommandPool.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <cstring>

namespace LunaraEngine
{
    VulkanStorageBuffer::VulkanStorageBuffer(RendererDataType* rendererData, uint8_t* data, size_t length,
                                             size_t stride)
    {
        Create(rendererData, data, length, stride);
    }

    void VulkanStorageBuffer::Create(RendererDataType* rendererData, uint8_t* data, size_t length, size_t stride)
    {
        m_ResourceType = ShaderResourceType::StorageBuffer;
        m_Device = rendererData->device;
        m_Size = length * stride;
        m_Stride = stride;

        CreateBuffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
        BindBufferToDevMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              rendererData->physicalDevice);
        if (data != nullptr) { Upload(data, length, stride); }
    }


}// namespace LunaraEngine