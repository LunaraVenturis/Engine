#include <LunaraEngine/Renderer/Vulkan/Buffer/TextureBuffer.hpp>
#include <LunaraEngine/Renderer/Vulkan/VulkanDataTypes.hpp>
#include "TextureBuffer.hpp"

namespace LunaraEngine
{
    VulkanTextureBuffer::VulkanTextureBuffer(RendererDataType* rendererData, VkQueue executeQueue,
                                             const TextureInfo& info, TextureDataView* dataView)
    {
        Create(rendererData, executeQueue, info, dataView);
    }

    void VulkanTextureBuffer::Create(RendererDataType* rendererData, VkQueue executeQueue, const TextureInfo& info,
                                     TextureDataView* dataView)
    {
        m_ResourceType = BufferResourceType::Texture;
        m_Device = rendererData->device;
        m_Dimensions.width = info.width;
        m_Dimensions.height = info.height;
        m_Stride = static_cast<size_t>(info.format);

        m_Resource.width = info.width;
        m_Resource.height = info.height;
        m_Resource.path = info.path;
        m_Resource.textureNames = {info.name};
        m_Resource.channelDepth = info.channelDepth;
        m_Resource.format = info.format;
        m_Resource.type = info.type;
        m_Resource.layerCount = 1;
        m_Resource.textureType = TextureResourceType::Texture2D;
        LogInfo(info);

        VkFormat format = GetFormat();
        CreateImage2D(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, info.width, info.height, format,
                      VK_IMAGE_TILING_OPTIMAL);

        BindBufferToDevMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, rendererData->physicalDevice);

        {
            StagingBuffer stagingBuffer(rendererData->device, rendererData->physicalDevice, dataView->data,
                                        info.width * info.height, m_Stride);
            VulkanFence fence(rendererData->device);

            auto cmdBuffer = stagingBuffer.BeginRecording(rendererData->commandPool);
            TransitionLayout(cmdBuffer.get(), format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            stagingBuffer.CopyTo(cmdBuffer.get(), (Buffer<BufferResourceType::Texture>*) this);
            TransitionLayout(cmdBuffer.get(), format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            stagingBuffer.Submit(cmdBuffer.get(), executeQueue, &fence);
            stagingBuffer.Destroy();
            fence.Destroy();
        }

        CreateImageView();
        CreateSampler();
    }

    void VulkanTextureBuffer::Create(RendererDataType* rendererData, VkQueue executeQueue,
                                     TextureResource& shaderResource, std::vector<TextureDataView>& dataViews)
    {
        m_ResourceType = BufferResourceType::Texture;
        m_Device = rendererData->device;
        m_Stride = static_cast<size_t>(TextureFormat::RGBA);
        m_Dimensions.width = shaderResource.width;
        m_Dimensions.height = shaderResource.height;
        m_Resource = shaderResource;

        LogInfo(shaderResource);

        VkFormat format = GetFormat();
        CreateImage2DArray(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, shaderResource.width,
                           shaderResource.height, static_cast<uint32_t>(shaderResource.layerCount), format,
                           VK_IMAGE_TILING_OPTIMAL);

        BindBufferToDevMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, rendererData->physicalDevice);

        std::vector<StagingBuffer> stagingBuffers;

        for (uint32_t i = 0; i < static_cast<uint32_t>(shaderResource.layerCount); ++i)
        {
            // Copy textures data to staging buffers
            stagingBuffers.emplace_back(rendererData->device, rendererData->physicalDevice, dataViews[i].data,
                                        shaderResource.width * shaderResource.height, m_Stride);

            // Copy textures data from staging buffers to texture buffer
            VulkanFence fence(rendererData->device);

            auto& stagingBuffer = stagingBuffers[i];

            auto cmdBuffer = stagingBuffer.BeginRecording(rendererData->commandPool);
            TransitionLayout(
                    cmdBuffer.get(), format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, i);
            stagingBuffer.CopyTo(cmdBuffer.get(), (Buffer<BufferResourceType::Texture>*) this, 0, i);
            TransitionLayout(cmdBuffer.get(), format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i);
            stagingBuffer.Submit(cmdBuffer.get(), executeQueue, &fence);
            stagingBuffer.Destroy();
            fence.Destroy();
        }

        CreateImageView();
        CreateSampler();
    }

    VkImageView VulkanTextureBuffer::GetView() const { return m_ImageView; }

    VkSampler VulkanTextureBuffer::GetSampler() const { return m_Sampler; }

    VkFormat VulkanTextureBuffer::GetFormat() const
    {
        switch (m_Resource.format)
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

        switch (m_Resource.textureType)
        {
            case TextureResourceType::Texture2D:
                createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                break;
            case TextureResourceType::Texture2DArray:
                createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                break;
            case TextureResourceType::Texture3D:
                createInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
            default:
                break;
        }

        createInfo.format = GetFormat();
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = m_Resource.layerCount;
        if (vkCreateImageView(m_Device, &createInfo, nullptr, &m_ImageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        }
    }

    void VulkanTextureBuffer::CreateSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_NEAREST;
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

    void VulkanTextureBuffer::LogInfo(const TextureInfo& info) const
    {
        LOG_DEBUG("Creating Texture Buffer");
        LOG_DEBUG("\tName: %ls", info.name.data());
        LOG_DEBUG("\tWidth: %u", info.width);
        LOG_DEBUG("\tHeight: %u", info.height);
        LOG_DEBUG("\tDepth: %u", info.channelDepth);
        LOG_DEBUG("\tLength: %u", info.width * info.height);
        LOG_DEBUG("\tStride: %zu", m_Stride);
    }

    void VulkanTextureBuffer::LogInfo(const TextureResource& resource) const
    {
        LOG_DEBUG("Creating Texture Array Buffer");
        LOG_DEBUG("\tPath: %ls", resource.path.wstring().data());
        LOG_DEBUG("\tLayer Count: %u", resource.layerCount);
        LOG_DEBUG("\tWidth: %u", resource.width);
        LOG_DEBUG("\tHeight: %u", resource.height);
        LOG_DEBUG("\tDepth: %u", resource.channelDepth);
        LOG_DEBUG("\tLength: %u", resource.width * resource.height);
        LOG_DEBUG("\tStride: %zu", m_Stride);
        LOG_DEBUG("\tIndividual Texture Size: %zu", resource.width * resource.height * m_Stride);
        LOG_DEBUG("\tTotal Size: %zu", resource.width * resource.height * m_Stride * resource.layerCount);
        LOG_DEBUG("\tNames:");
        for (auto& textureName: resource.textureNames) { LOG_DEBUG("\t\tName: %ls", textureName.data()); }
    }


}// namespace LunaraEngine