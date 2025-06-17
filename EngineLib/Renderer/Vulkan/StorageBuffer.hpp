#pragma once

#include "Buffer.hpp"
#include "StagingBuffer.hpp"
#include <glm/glm.hpp>
#include <array>
#include <vector>

namespace LunaraEngine
{

    class CommandPool;
    struct RendererDataType;

    class VulkanStorageBuffer: public Buffer
    {
    public:
        VulkanStorageBuffer() = default;
        VulkanStorageBuffer(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                            size_t stride = 1);
        ~VulkanStorageBuffer() = default;

    public:
        void Upload(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                    size_t stride = 1);

        void Create(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                    size_t stride = 1);

    private:
        StagingBuffer m_StagingBuffer;
    };
}// namespace LunaraEngine