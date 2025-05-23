#include "Buffer.hpp"
#include "Common.hpp"
#include <vector>
#include <array>

#pragma once

namespace LunaraEngine
{


    const std::vector<Vertex> vertices = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                          {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                          {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    class VertexBuffer: public Buffer
    {
    public:
        VertexBuffer();
        VertexBuffer(VkDeviceSize size);
        ~VertexBuffer(); 
    public:
        void Upload(const VertexBuffer* buffer);
    private:
        VkDevice m_Device{};
        VkDeviceSize m_Size{};
        VkDeviceMemory m_VertexBufferMemory{};
    };
}// namespace LunaraEngine