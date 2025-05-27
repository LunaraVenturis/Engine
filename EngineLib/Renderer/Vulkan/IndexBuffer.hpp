#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include "Buffer.hpp"
#include "StagingBuffer.hpp"

namespace LunaraEngine
{
    class CommandPool;

    class IndexBuffer: public Buffer
    {
    public:
        IndexBuffer() = default;
        IndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, CommandPool* commandPool, VkQueue executeQueue,
                    uint8_t* data, size_t size);
        ~IndexBuffer() = default;
    public:
        void Create(VkDevice device, VkPhysicalDevice physicalDevice, CommandPool* commandPool, VkQueue executeQueue,
                    uint8_t* data, size_t size);
        [[nodiscard]] static const auto& GetIndices() { return m_Indices; }
    private:
        StagingBuffer m_StagingBuffer;
        inline static const std::vector<uint16_t> m_Indices = {0, 1, 2, 2, 3, 0};
    };
}// namespace LunaraEngine