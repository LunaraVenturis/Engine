#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <Renderer/Vulkan/Buffer/Buffer.hpp>
#include <Renderer/Vulkan/Buffer/StagingBuffer.hpp>

namespace LunaraEngine
{
    class CommandPool;
    struct RendererDataType;

    class VulkanTextureBuffer: public Buffer<ShaderResourceType::Texture>
    {
    public:
        VulkanTextureBuffer() = default;
        VulkanTextureBuffer(RendererDataType* rendererData, VkQueue executeQueue, const TextureInfo& info,
                            uint8_t* data);
        ~VulkanTextureBuffer() = default;

    public:
        void Create(RendererDataType* rendererData, VkQueue executeQueue, const TextureInfo& info, uint8_t* data);

    private:
        VkFormat GetFormat() const;

    private:
        TextureInfo m_Info;
        StagingBuffer m_StagingBuffer;
    };
}// namespace LunaraEngine