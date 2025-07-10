#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <Renderer/Vulkan/Buffer/Buffer.hpp>
#include <Renderer/Vulkan/Buffer/StagingBuffer.hpp>

namespace LunaraEngine
{

    class TextureReader
    {
    public:
        TextureReader() = default;
        ~TextureReader() = default;

    public:
        void Open(const std::filesystem::path& path, std::wstring name);

        uint8_t* Read();

        void Close();

        TextureInfo GetInfo() const;

    private:
        uint8_t* m_Data;
        TextureInfo m_Info;
    };

    class CommandPool;
    struct RendererDataType;

    class VulkanTextureBuffer: public Buffer<BufferResourceType::Texture>
    {
    public:
        VulkanTextureBuffer() = default;
        VulkanTextureBuffer(RendererDataType* rendererData, VkQueue executeQueue, const TextureInfo& info,
                            uint8_t* data);
        ~VulkanTextureBuffer() = default;

    public:
        void Create(RendererDataType* rendererData, VkQueue executeQueue, const TextureInfo& info, uint8_t* data);
        VkImageView GetView() const;
        VkSampler GetSampler() const;

    private:
        VkFormat GetFormat() const;
        void CreateImageView();
        void CreateSampler();

    private:
        TextureInfo m_Info;
        StagingBuffer m_StagingBuffer;
        VkImageView m_ImageView;
        VkSampler m_Sampler;
    };
}// namespace LunaraEngine