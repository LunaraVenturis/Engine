#pragma once

/**
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @date 01.10.2024
 * 
 * @section DESCRIPTION
 * 
 * UniformBuffer class definition
 */


#include <Renderer/MemoryBuffer.hpp>

#include <Core/Result.hpp>
#include <vulkan/vulkan.h>

namespace Engine
{

    class UniformBuffer: public MemoryBuffer
    {
    public:
        UniformBuffer() = default;
        ~UniformBuffer() = default;

    public:
        Engine::ResultValueType<BufferStatus> Init(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t size,
                                                   uint8_t* data);
        Engine::ResultValue<BufferStatus, VkBuffer> GetBufferHandle();
        Engine::ResultValueType<BufferStatus> Destroy(VkDevice device);

        const size_t GetSize() const;

    private:
        size_t m_Size{};
        VkBuffer m_Buffer{};
        VkDeviceMemory m_BufferMemory{};
    };
}// namespace Engine