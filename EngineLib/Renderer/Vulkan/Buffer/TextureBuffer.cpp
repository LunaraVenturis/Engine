#include <Renderer/Vulkan/Buffer/TextureBuffer.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include "TextureBuffer.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma GCC diagnostic pop

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
        m_ResourceType = BufferResourceType::Texture;
        m_Device = rendererData->device;
        m_Dimensions.width = info.width;
        m_Dimensions.height = info.height;
        m_Stride = static_cast<size_t>(info.format);

        VkFormat format = GetFormat();

        LOG_DEBUG("Creating Texture Buffer");
        LOG_DEBUG("\tName: %ls", info.name.c_str());
        LOG_DEBUG("\tWidth: %u", info.width);
        LOG_DEBUG("\tHeight: %u", info.height);
        LOG_DEBUG("\tDepth: %u", info.channelDepth);
        LOG_DEBUG("\tLength: %u", info.width * info.height);
        LOG_DEBUG("\tStride: %zu", m_Stride);

        m_StagingBuffer.Create(
                rendererData->device, rendererData->physicalDevice, data, info.width * info.height, m_Stride);

        CreateImage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, info.width, info.height, format,
                    VK_IMAGE_TILING_OPTIMAL);

        BindBufferToDevMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, rendererData->physicalDevice);

        {
            VulkanFence fence(rendererData->device);

            auto cmdBuffer = m_StagingBuffer.BeginRecording(rendererData->commandPool);
            TransitionLayout(cmdBuffer.get(), format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            m_StagingBuffer.CopyTo(cmdBuffer.get(), (Buffer<BufferResourceType::Texture>*) this);
            TransitionLayout(cmdBuffer.get(), format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            m_StagingBuffer.Submit(cmdBuffer.get(), executeQueue, &fence);
            m_StagingBuffer.Destroy();
            fence.Destroy();
        }

        CreateImageView();
        CreateSampler();
    }

    VkImageView VulkanTextureBuffer::GetView() const { return m_ImageView; }

    VkSampler VulkanTextureBuffer::GetSampler() const { return m_Sampler; }

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

    void VulkanTextureBuffer::CreateImageView()
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_Image;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = GetFormat();
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(m_Device, &createInfo, nullptr, &m_ImageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        }
    }

    void VulkanTextureBuffer::CreateSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.anisotropyEnable = VK_FALSE;//DISABLE FOR NOW
        samplerInfo.maxAnisotropy = 0;          // DISABLE FOR NOW
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
        if (vkCreateSampler(m_Device, &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void TextureReader::Open(const std::filesystem::path& path, std::wstring name)
    {
        m_Info.path = path;
        m_Info.name = name;
        m_Info.format = TextureFormat::RGBA;
        m_Info.type = TextureDataType::Int;

        std::string fileName((path / name).string());

        if (!std::filesystem::exists(fileName)) { throw std::runtime_error("file does not exist!"); }

        uint8_t channels;
        m_Data = stbi_load(fileName.c_str(), (int*) &m_Info.width, (int*) &m_Info.height, (int*) &channels, 4);
        assert(m_Data != nullptr);
        if (m_Info.width == 0 || m_Info.height == 0) { LOG_ERROR("%s", stbi_failure_reason()); }
        assert(m_Info.width > 0 && m_Info.height > 0);
        if (channels != 4) { LOG_ERROR("%s", stbi_failure_reason()); }
        assert(channels == 4);
    }

    uint8_t* TextureReader::Read() { return m_Data; }

    TextureInfo TextureReader::GetInfo() const { return m_Info; }

    void TextureReader::Close()
    {
        stbi_image_free(m_Data);
        m_Data = nullptr;
    }

}// namespace LunaraEngine