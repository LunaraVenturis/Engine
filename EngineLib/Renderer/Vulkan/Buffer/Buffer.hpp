#pragma once
#include <vulkan/vulkan.h>
#include <Renderer/CommonTypes.hpp>
#include <cassert>

namespace LunaraEngine
{
    class CommandPool;
    class CommandBuffer;
    class VulkanFence;

    template <BufferResourceType type = BufferResourceType::None>
    class Buffer
    {
    public:
        Buffer() = default;
        virtual ~Buffer();

    public:
        [[nodiscard]] auto GetHandle() const
        {
            if constexpr (type == BufferResourceType::Texture) { return m_Image; }
            else { return m_Buffer; }
        }

        [[nodiscard]] size_t GetLength() const { return m_Size / m_Stride; }

        [[nodiscard]] size_t GetWidth() const { return m_Dimensions.width; }

        [[nodiscard]] size_t GetHeight() const { return m_Dimensions.height; }

        [[nodiscard]] size_t GetStride() const { return m_Stride; }

        [[nodiscard]] bool IsValid() const { return m_Buffer != VK_NULL_HANDLE; }

        void Upload(size_t offset, uint8_t* data, size_t length, size_t stride = 1);
        void Upload(uint8_t* data, size_t length, size_t stride = 1);
        std::shared_ptr<CommandBuffer> BeginRecording(CommandPool* commandPool);
        void Submit(CommandBuffer* cmdBuffer, VkQueue executeQueue, VulkanFence* fence);

        template <BufferResourceType U>
        void CopyTo(CommandPool* commandPool, VkQueue executeQueue, Buffer<U>* buffer);
        template <BufferResourceType U>
        void CopyTo(CommandBuffer* cmdBuffer, Buffer<U>* buffer);

        void TransitionLayout(CommandBuffer* cmdBuffer, VkFormat format, VkImageLayout oldLayout,
                              VkImageLayout newLayout) const requires(type == BufferResourceType::Texture);

        void Destroy();

        BufferResourceType GetResourceType() const;

    protected:
        void CreateBuffer(VkBufferUsageFlags usage)
        {
            if constexpr (type != BufferResourceType::Texture)
            {
                VkBufferCreateInfo bufferInfo{};
                bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferInfo.size = m_Size * m_Stride;
                bufferInfo.usage = usage;
                bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                auto result = vkCreateBuffer(m_Device, &bufferInfo, nullptr, &m_Buffer);
                assert(result == VK_SUCCESS);
                (void) result;
            }
        }

        void CreateImage(VkImageUsageFlags usage, uint32_t width, uint32_t height, VkFormat format,
                         VkImageTiling tiling)
        {
            if constexpr (type == BufferResourceType::Texture)
            {
                VkImageCreateInfo imageInfo{};
                imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
                imageInfo.extent.width = width;
                imageInfo.extent.height = height;
                imageInfo.extent.depth = 1;
                imageInfo.mipLevels = 1;
                imageInfo.arrayLayers = 1;
                imageInfo.format = format;
                imageInfo.tiling = tiling;
                imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageInfo.usage = usage;
                imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                auto result = vkCreateImage(m_Device, &imageInfo, nullptr, &m_Image);
                assert(result == VK_SUCCESS);
                (void) result;
            }
        }

        void BindBufferToDevMemory(VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);

    protected:
        VkDevice m_Device{};

        union
        {
            VkBuffer m_Buffer;
            VkImage m_Image;
        };

        union
        {
            size_t m_Size;

            struct {
                uint32_t width;
                uint32_t height;
            } m_Dimensions;
        };

        size_t m_Stride{};
        VkDeviceMemory m_BufferMemory{};
        uint8_t* m_MappedDataPtr{};
        BufferResourceType m_ResourceType{};
    };


}// namespace LunaraEngine

#include "Buffer.inl"