#include <Renderer/Vulkan/Buffer/TextureBuffer.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include "TextureBuffer.hpp"

namespace LunaraEngine
{
    VulkanTextureBuffer::VulkanTextureBuffer(RendererDataType* rendererData, VkQueue executeQueue,
                                             const TextureInfo& info, uint8_t* data)
    {
        Create(rendererData, executeQueue, info, data);
    }

    void VulkanTextureBuffer::Create(RendererDataType* rendererData, VkQueue executeQueue, const TextureInfo& info,
                                     uint8_t* data)
    {
        m_ResourceType = ShaderResourceType::UniformBuffer;
        m_Device = rendererData->device;
        m_Dimensions.width = info.width;
        m_Dimensions.height = info.height;
        m_Stride = info.channelDepth * static_cast<size_t>(info.format);

        VkFormat format = GetFormat();

        m_StagingBuffer.Create(rendererData->device, rendererData->physicalDevice, data, info.width * info.height,
                               info.channelDepth * static_cast<size_t>(info.format));

        {
            VulkanFence fence(rendererData->device);
            auto cmdBuffer = m_StagingBuffer.BeginRecording(rendererData->commandPool);
            m_StagingBuffer.CopyTo(cmdBuffer.get(), (Buffer<ShaderResourceType::Texture>*) this);
            m_StagingBuffer.Submit(cmdBuffer.get(), executeQueue, &fence);
            fence.Destroy();
        }

        {
            VulkanFence fence(rendererData->device);
            CreateImage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, info.width, info.height, format,
                        VK_IMAGE_TILING_OPTIMAL);

            BindBufferToDevMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, rendererData->physicalDevice);

            auto cmdBuffer = m_StagingBuffer.BeginRecording(rendererData->commandPool);
            TransitionLayout(cmdBuffer.get(), format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            m_StagingBuffer.CopyTo(cmdBuffer.get(), (Buffer<ShaderResourceType::Texture>*) this);
            TransitionLayout(cmdBuffer.get(), format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_StagingBuffer.Submit(cmdBuffer.get(), executeQueue, &fence);
            m_StagingBuffer.Destroy();
            fence.Destroy();
        }
    }

    VkFormat VulkanTextureBuffer::GetFormat() const
    {
        switch (m_Info.format)
        {
            case TextureFormat::RGB:
                return VK_FORMAT_R8G8B8_UNORM;
            case TextureFormat::RGBA:
                return VK_FORMAT_R8G8B8A8_UNORM;
            default:
                return VK_FORMAT_R8G8B8A8_UNORM;
        }
    }
}// namespace LunaraEngine