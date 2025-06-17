#pragma once
#include "vulkan/vulkan.h"
#include <Renderer/CommonTypes.hpp>

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

        [[nodiscard]] size_t GetLength() const { return m_Size / m_Stride; }

        [[nodiscard]] size_t GetStride() const { return m_Stride; }

        [[nodiscard]] bool IsValid() const { return m_Buffer != VK_NULL_HANDLE; }

        void Upload(uint8_t* data, size_t length, size_t stride = 1);
        void CopyTo(CommandPool* commandPool, VkQueue executeQueue, Buffer* buffer);
        void Destroy();

        ShaderResourceType GetResourceType() const;

    protected:
        void CreateBuffer(VkBufferUsageFlags usage);
        void BindBufferToDevMemory(VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);

    protected:
        VkDevice m_Device{};
        VkBuffer m_Buffer{};
        VkDeviceMemory m_BufferMemory{};
        size_t m_Size{};
        size_t m_Stride{};
        ShaderResourceType m_ResourceType;
    };
}// namespace LunaraEngine