#pragma once
#include <vulkan/vulkan.h>

namespace LunaraEngine
{

    class Queue
    {
    public:
        Queue() = default;
        ~Queue() = default;

        Queue(VkQueue queue, uint32_t queueFamilyIndex) : m_Queue(queue), m_QueueFamilyIndex(queueFamilyIndex) {}

        Queue(VkQueue queue) : m_Queue(queue), m_QueueFamilyIndex(0) {}

    public:
        uint32_t GetIndex() const { return m_QueueFamilyIndex; }

    public:
        operator VkQueue() const { return m_Queue; }

        operator VkQueue&() { return m_Queue; }

    private:
        VkQueue m_Queue{};
        uint32_t m_QueueFamilyIndex{};
    };

}// namespace LunaraEngine