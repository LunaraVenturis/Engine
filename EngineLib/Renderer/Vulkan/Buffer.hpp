#include "vulkan/vulkan.h"
#pragma once
namespace LunaraEngine
{
    class Buffer
    {
    public:
        Buffer(VkDevice Device);
    protected:
        void CreateBuffer(VkBuffer& buffer, VkDeviceSize size, VkBufferUsageFlags usage);
        void BindBufferToDevMemory(VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkMemoryPropertyFlags properties);
    private:
        VkDevice m_Device{};
    };
}// namespace LunaraEngine