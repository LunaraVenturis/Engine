/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * UniformBuffer class implementation
 */


#include "UniformBuffer.hpp"
#include <Core/Log.hpp>

namespace Engine
{
    ResultValueType<BufferStatus> UniformBuffer::Init(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t size,
                                                      uint8_t* data)
    {
        VkBufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = size;
        createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        auto uniformDataBufferStatus = vkCreateBuffer(device, &createInfo, nullptr, &m_Buffer);
        if (VK_SUCCESS != uniformDataBufferStatus)
        {
            LOG_ERROR("Can Not Create Uniform Buffer!\n");
            return ResultValueType{BufferStatus::Fail};
        }

        VkPhysicalDeviceMemoryProperties memoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
        VkMemoryRequirements memoryRequirements{};
        vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);

        auto allocateBufferStatus =
                InitMemoryBuffer(physicalDevice, device, memoryProperties, memoryRequirements,
                                 (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

        if (BufferStatus::Success != allocateBufferStatus) { return allocateBufferStatus; }

        auto copyStatus = CopyData(device, data, size);
        if (BufferStatus::Success != copyStatus) { return copyStatus; }

        auto bindStatus = BindBuffer(device, m_Buffer);
        if (BufferStatus::Bound != bindStatus) { return bindStatus; }

        return ResultValueType{BufferStatus::Success};
    }

    ResultValue<BufferStatus, VkBuffer> UniformBuffer::GetBufferHandle()
    {
        if (VK_NULL_HANDLE == m_Buffer)
        {
            return ResultValue<BufferStatus, VkBuffer>{BufferStatus::Fail, VK_NULL_HANDLE};
        }
        return ResultValue<BufferStatus, VkBuffer>{BufferStatus::Success, m_Buffer};
    }

    ResultValueType<BufferStatus> UniformBuffer::Destroy(VkDevice device)
    {
        if (VK_NULL_HANDLE == m_Buffer) { return ResultValueType{BufferStatus::Fail}; }
        vkDestroyBuffer(device, m_Buffer, nullptr);

        return DestroyMemoryBuffer(device);
    }

    const size_t UniformBuffer::GetSize() const { return m_Size; }


}// namespace Engine