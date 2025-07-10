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

    class VulkanIndexBuffer: public Buffer<BufferResourceType::Buffer>
    {
    public:
        VulkanIndexBuffer() = default;
        VulkanIndexBuffer(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                          size_t stride = 1);
        ~VulkanIndexBuffer() = default;

    public:
        void Create(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                    size_t stride = 1);

        void Upload(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                    size_t stride = 1);

    private:
        StagingBuffer m_StagingBuffer;
    };
}// namespace LunaraEngine