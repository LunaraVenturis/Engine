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

    public:
        [[nodiscard]] auto GetVertexBuffer() const { return m_VertexBuffer; }

    private:
        VkDevice m_Device{};
        VkDeviceSize m_Size{};
        VkBuffer m_VertexBuffer{};
        VkDeviceMemory m_VertexBufferMemory{};
    };
}// namespace LunaraEngine