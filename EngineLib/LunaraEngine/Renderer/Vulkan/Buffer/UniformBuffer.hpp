#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include <LunaraEngine/Renderer/Vulkan/VulkanDataTypes.hpp>
#include <LunaraEngine/Renderer/Vulkan/Buffer/Buffer.hpp>
#include <LunaraEngine/Renderer/Vulkan/Buffer/StagingBuffer.hpp>

namespace LunaraEngine
{
    class CommandPool;
    struct RendererDataType;

    class VulkanUniformBuffer: public Buffer<BufferResourceType::Buffer>
    {
    public:
        VulkanUniformBuffer() = default;
        VulkanUniformBuffer(RendererDataType* rendererData, uint8_t* data, size_t length, size_t stride = 1);
        ~VulkanUniformBuffer() = default;

    public:
        void Create(RendererDataType* rendererData, uint8_t* data, size_t length, size_t stride = 1);
    };
}// namespace LunaraEngine