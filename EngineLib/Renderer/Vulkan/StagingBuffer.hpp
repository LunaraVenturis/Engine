#pragma once
#include "vulkan/vulkan.h"
#include "Buffer.hpp"
#include "VertexBuffer.hpp"
namespace LunaraEngine
{
    class StagingBuffer : public Buffer
    {
    public:
        StagingBuffer(VkDevice device);
        StagingBuffer(VkDevice device, VkPhysicalDevice physicalDevice);
    public:
        void Upload(VkPhysicalDevice physicalDevice);
    private:
        VkDevice m_Device{};
        VkDeviceSize m_Size{};
        VkDeviceMemory m_StagingBufferMemory{};
    };
}// namespace LunaraEngine