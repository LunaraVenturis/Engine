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
    struct RendererDataType;

    class VulkanVertexBuffer: public Buffer<BufferResourceType::Buffer>
    {
    public:
        VulkanVertexBuffer() = default;
        VulkanVertexBuffer(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                           size_t stride = 1);
        ~VulkanVertexBuffer() = default;

    public:
        void Upload(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                    size_t stride = 1);

        void Create(RendererDataType* rendererData, VkQueue executeQueue, uint8_t* data, size_t length,
                    size_t stride = 1);

    private:
        StagingBuffer m_StagingBuffer;
    };
}// namespace LunaraEngine