#pragma once
#include "vulkan/vulkan.h"

namespace LunaraEngine
{
    class CommandPool;

    class Buffer
    {
    public:
        Buffer() = default;
        virtual ~Buffer();

    public:
        [[nodiscard]] auto GetBuffer() const { return m_Buffer; }

        void Upload(uint8_t* data, size_t size);
        void CopyTo(CommandPool* commandPool, VkQueue executeQueue, Buffer* buffer);
        void Destroy();

    protected:
        void CreateBuffer(VkBufferUsageFlags usage);
        void BindBufferToDevMemory(VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);

        const auto& GetDevice() const { return m_Device; }

    protected:
        VkDevice m_Device{};
        VkBuffer m_Buffer{};
        VkDeviceMemory m_BufferMemory{};
        size_t m_Size{};
    };
}// namespace LunaraEngine