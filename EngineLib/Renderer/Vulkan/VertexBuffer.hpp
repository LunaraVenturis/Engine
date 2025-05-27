#pragma once
#include "Buffer.hpp"
#include "StagingBuffer.hpp"
#include <glm/glm.hpp>
#include <array>
#include <vector>

namespace LunaraEngine
{

    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            return attributeDescriptions;
        }
    };

    class CommandPool;

    class VertexBuffer: public Buffer
    {
    public:
        VertexBuffer() = default;
        VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, CommandPool* commandPool, VkQueue executeQueue,
                     uint8_t* data, size_t size);
        ~VertexBuffer() = default;

    public:
        [[nodiscard]] static const auto& GetVertices() { return m_Vertices; }

        void Create(VkDevice device, VkPhysicalDevice physicalDevice, CommandPool* commandPool, VkQueue executeQueue,
                    uint8_t* data, size_t size);

    private:
        StagingBuffer m_StagingBuffer;
        inline static const std::vector<Vertex> m_Vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                                              {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                                              {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                                              {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};
    };
}// namespace LunaraEngine