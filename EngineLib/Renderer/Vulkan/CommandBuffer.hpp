#pragma once
#include <vulkan/vulkan.h>

namespace LunaraEngine
{
    class CommandBuffer
    {
    public:
        CommandBuffer(VkDevice device, VkCommandPool cmdPool);
        ~CommandBuffer();
        CommandBuffer(const CommandBuffer& other) = delete;
        CommandBuffer(CommandBuffer&& other);
        
    public:
        void Destroy();
        void BeginRecording();
        void EndRecording();
        bool IsValid() const;
        
        void Draw();
        void Flush(VkQueue queue);
    public:
        operator VkCommandBuffer() const;

    private:
        VkDevice m_device{};
        VkCommandBuffer m_cmdBuffer{};
        VkCommandPool m_cmdPool{};
    };
}// namespace LunaraEngine