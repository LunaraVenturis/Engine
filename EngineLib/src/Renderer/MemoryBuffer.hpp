#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * Buffer class definition
 */


#include <Core/Result.hpp>
#include <vulkan/vulkan.h>

namespace Engine
{
    enum class BufferStatus
    {
        Fail,
        CanNotFindMemoryType,
        CanNotAllocateMemory,
        CanNotBindMemory,
        CanNotMapMemory,
        Bound,
        Success
    };
}// namespace Engine

namespace Engine
{

    class MemoryBuffer
    {
    public:
        MemoryBuffer() = default;
        ~MemoryBuffer() = default;

        virtual ResultValueType<BufferStatus> InitMemoryBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                                                               VkPhysicalDeviceMemoryProperties& memoryProperties,
                                                               VkMemoryRequirements& memoryRequirements,
                                                               VkMemoryPropertyFlags flags);

        virtual ResultValueType<BufferStatus> BindBuffer(VkDevice device, VkBuffer buffer);
        virtual ResultValueType<BufferStatus> BindImage(VkDevice device, VkImage image);

        virtual ResultValueType<BufferStatus> CopyData(VkDevice device, uint8_t* data, size_t size);

        virtual ResultValueType<BufferStatus> DestroyMemoryBuffer(VkDevice device);

        virtual ResultValue<BufferStatus, uint32_t>
        FindMemoryTypeIndex(VkPhysicalDeviceMemoryProperties& memoryProperties,
                            VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags propertyFlags);

        virtual ResultValue<BufferStatus, VkDeviceMemory>
        AllocateMemory(VkPhysicalDevice physicalDevice, VkDevice device,
                       VkPhysicalDeviceMemoryProperties& memoryProperties, VkMemoryRequirements& memoryRequirements,
                       VkMemoryPropertyFlags memoryMask);

        const size_t GetBufferSize() const;

    protected:
        size_t m_BufferSize{};
        VkDeviceMemory m_BufferMemory{};
    };
}// namespace Engine