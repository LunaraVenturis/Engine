/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * MemoryBuffer class implementation
 */


#include "MemoryBuffer.hpp"
#include <Core/Log.hpp>
#include <vulkan/vulkan.h>

namespace Engine
{

    ResultValueType<BufferStatus> MemoryBuffer::InitMemoryBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                                                                 VkPhysicalDeviceMemoryProperties& memoryProperties,
                                                                 VkMemoryRequirements& memoryRequirements,
                                                                 VkMemoryPropertyFlags flags)
    {
        if (memoryRequirements.size <= 0) { return {BufferStatus::CanNotAllocateMemory}; }

        auto allocationStatus = AllocateMemory(physicalDevice, device, memoryProperties, memoryRequirements, flags);
        if (BufferStatus::Success != allocationStatus) { return {allocationStatus.status}; }

        m_BufferSize = memoryRequirements.size;

        return ResultValueType<BufferStatus>{BufferStatus::Success};
    }

    ResultValueType<BufferStatus> MemoryBuffer::BindBuffer(VkDevice device, VkBuffer buffer)
    {
        auto bindStatus = vkBindBufferMemory(device, buffer, m_BufferMemory, 0);
        if (VK_SUCCESS != bindStatus)
        {
            LOG_ERROR("Can Not Bind Buffer Memory!\n");
            return ResultValueType{BufferStatus::CanNotBindMemory};
        }
        return ResultValueType<BufferStatus>{BufferStatus::Bound};
    }

    ResultValueType<BufferStatus> MemoryBuffer::BindImage(VkDevice device, VkImage image)
    {
        auto bindStatus = vkBindImageMemory(device, image, m_BufferMemory, 0);
        if (VK_SUCCESS != bindStatus)
        {
            LOG_ERROR("Can Not Bind Image Memory!\n");
            return ResultValueType{BufferStatus::CanNotBindMemory};
        }
        return ResultValueType<BufferStatus>{BufferStatus::Bound};
    }

    ResultValueType<BufferStatus> MemoryBuffer::CopyData(VkDevice device, uint8_t* data, size_t size)
    {
        if (size > m_BufferSize) { return {BufferStatus::Fail}; }

        uint8_t* pData{};
        auto mapMemoryStatus = vkMapMemory(device, m_BufferMemory, 0, size, 0, (void**) &pData);
        if (VK_SUCCESS != mapMemoryStatus)
        {
            LOG_ERROR("Can Not Map Buffer Memory!\n");
            return ResultValueType{BufferStatus::CanNotMapMemory};
        }

        memcpy(pData, &data, size);
        vkUnmapMemory(device, m_BufferMemory);

        return ResultValueType<BufferStatus>{BufferStatus::Success};
    }

    ResultValueType<BufferStatus> MemoryBuffer::DestroyMemoryBuffer(VkDevice device)
    {
        if (VK_NULL_HANDLE != m_BufferMemory) { return ResultValueType{BufferStatus::Fail}; }
        vkFreeMemory(device, m_BufferMemory, nullptr);

        return ResultValueType{BufferStatus::Success};
    }

    ResultValue<BufferStatus, uint32_t>
    MemoryBuffer::FindMemoryTypeIndex(VkPhysicalDeviceMemoryProperties& memoryProperties,
                                      VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags propertyFlags)
    {
        uint32_t typeBits = memoryRequirements.memoryTypeBits;
        uint32_t typeIndex = uint32_t(~0);
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if ((typeBits & 1) && ((memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags))
            {
                typeIndex = i;
                break;
            }
            typeBits >>= 1;
        }

        if (typeIndex == uint32_t(~0))
        {
            LOG_ERROR("Can Not Find Correct Memory Type\n");
            return ResultValue<BufferStatus, uint32_t>{BufferStatus::CanNotFindMemoryType, 0};
        }
        return ResultValue<BufferStatus, uint32_t>{BufferStatus::Success, typeIndex};
    }

    ResultValue<BufferStatus, VkDeviceMemory>
    MemoryBuffer::AllocateMemory(VkPhysicalDevice physicalDevice, VkDevice device,
                                 VkPhysicalDeviceMemoryProperties& memoryProperties,
                                 VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryMask)
    {
        auto typeIndex = FindMemoryTypeIndex(memoryProperties, memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (BufferStatus::Success != typeIndex) { return ResultValue<BufferStatus, VkDeviceMemory>{typeIndex.status}; }


        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.pNext = NULL;
        memoryAllocateInfo.memoryTypeIndex = typeIndex.value;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;

        VkDeviceMemory memory;
        auto memoryStatus = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory);

        if (VK_SUCCESS != memoryStatus)
        {
            LOG_ERROR("Can Not Allocate Image Memory!\n");
            return ResultValue<BufferStatus, VkDeviceMemory>{BufferStatus::CanNotAllocateMemory};
        }

        return ResultValue<BufferStatus, VkDeviceMemory>{BufferStatus::Success, memory};
    }

    const size_t MemoryBuffer::GetBufferSize() const { return m_BufferSize; }

}// namespace Engine