#pragma once
#include "vulkan/vulkan.h"
#include "Buffer.hpp"

namespace LunaraEngine
{
    class StagingBuffer: public Buffer
    {
    public:
        StagingBuffer() = default;
        StagingBuffer(VkDevice device, VkPhysicalDevice physicalDevice, uint8_t* data, size_t size);

    public:
        void Create(VkDevice device, VkPhysicalDevice physicalDevice, uint8_t* data, size_t size);
    };
}// namespace LunaraEngine