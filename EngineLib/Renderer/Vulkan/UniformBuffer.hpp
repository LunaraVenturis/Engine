#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <Renderer/Vulkan/Buffer.hpp>
#include <Renderer/Vulkan/StagingBuffer.hpp>

namespace LunaraEngine
{
    class CommandPool;
    struct RendererDataType;

    class VulkanUniformBuffer: public Buffer
    {
    public:
        VulkanUniformBuffer() = default;
        VulkanUniformBuffer(RendererDataType* rendererData, uint8_t* data, size_t length, size_t stride = 1);
        ~VulkanUniformBuffer() = default;

    public:
        void Create(RendererDataType* rendererData, uint8_t* data, size_t length, size_t stride = 1);

        void Upload(RendererDataType* rendererData, uint8_t* data, size_t length, size_t stride = 1);
        void Upload(RendererDataType* rendererData, size_t offset, uint8_t* data, size_t size);
    };
}// namespace LunaraEngine