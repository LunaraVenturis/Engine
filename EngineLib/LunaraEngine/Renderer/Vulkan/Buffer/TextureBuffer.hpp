#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include <LunaraEngine/Renderer/TextureReader.hpp>
#include <LunaraEngine/Renderer/Vulkan/VulkanDataTypes.hpp>
#include <LunaraEngine/Renderer/Vulkan/Buffer/Buffer.hpp>
#include <LunaraEngine/Renderer/Vulkan/Buffer/StagingBuffer.hpp>
#include <expected>

namespace LunaraEngine
{
    class CommandPool;
    struct RendererDataType;

    class VulkanTextureBuffer: public Buffer<BufferResourceType::Texture>
    {
    public:
        VulkanTextureBuffer() = default;
        VulkanTextureBuffer(RendererDataType* rendererData, VkQueue executeQueue, const TextureInfo& info,
                            TextureDataView* dataView);

        ~VulkanTextureBuffer() = default;

    public:
        void Create(RendererDataType* rendererData, VkQueue executeQueue, const TextureInfo& info,
                    TextureDataView* dataView);

        void Create(RendererDataType* rendererData, VkQueue executeQueue, TextureResource& shaderResource,
                    std::vector<TextureDataView>& dataViews);


        VkImageView GetView() const;
        VkSampler GetSampler() const;

    private:
        void LogInfo(const TextureInfo& info) const;
        void LogInfo(const TextureResource& resource) const;
        VkFormat GetFormat() const;
        void CreateImageView();
        void CreateSampler();

    private:
        TextureResource m_Resource;
        VkImageView m_ImageView;
        VkSampler m_Sampler;
    };
}// namespace LunaraEngine