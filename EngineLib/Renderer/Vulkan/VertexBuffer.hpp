#include "Buffer.hpp"
#include "Common.hpp"


#pragma once

namespace LunaraEngine
{


    class VertexBuffer: public Buffer
    {
    public:
        VertexBuffer(VkDevice device);
        VertexBuffer(VkDevice device, VkDeviceSize size);
        ~VertexBuffer(); 
    public:
        void Upload(const VertexBuffer* buffer);
    private:
        VkDevice m_Device{};
        VkDeviceSize m_Size{};
        VkDeviceMemory m_VertexBufferMemory{};
    };
}// namespace LunaraEngine