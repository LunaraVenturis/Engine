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
        VulkanStorageBuffer(RendererDataType* rendererData, uint8_t* data, size_t length, size_t stride = 1);
        ~VulkanStorageBuffer() = default;

    public:
        void Create(RendererDataType* rendererData, uint8_t* data, size_t length, size_t stride = 1);
    };
}// namespace LunaraEngine