#pragma once
#include <vulkan/vulkan.h>
#include "Buffer.hpp"

namespace LunaraEngine
{
    class StagingBuffer: public Buffer<ShaderResourceType::Buffer>
    {
    public:
        StagingBuffer() = default;
        StagingBuffer(VkDevice device, VkPhysicalDevice physicalDevice, uint8_t* data, size_t length,
                      size_t stride = 1);

    public:
        void Create(VkDevice device, VkPhysicalDevice physicalDevice, uint8_t* data, size_t length, size_t stride = 1);
    };
}// namespace LunaraEngine