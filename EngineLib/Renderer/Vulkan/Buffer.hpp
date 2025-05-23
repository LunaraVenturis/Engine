#include "vulkan/vulkan.h"
#pragma once

namespace LunaraEngine
{
    class Buffer
    {
    public:
        Buffer(VkDevice Device);
       virtual ~Buffer();
    public:
        [[nodiscard]] auto GetBuffer() const { return m_Buffer; }

    protected:
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage);
        void BindBufferToDevMemory(VkDeviceMemory& bufferMemory, VkMemoryPropertyFlags properties);

    private:
        VkDevice m_Device{};
        VkBuffer m_Buffer{};
    };
}// namespace LunaraEngine