#include "VertexBuffer.hpp"
#include <Renderer/Vulkan/Buffer/VertexBuffer.hpp>
#include <Renderer/Vulkan/VulkanRendererAPI.hpp>

namespace LunaraEngine
{
    VertexBuffer::VertexBuffer(VertexAttributeLayout layout, size_t length) { Create(layout, nullptr, length); }

    VertexBuffer::VertexBuffer(VertexAttributeLayout layout, uint8_t* data, size_t length)
    {
        Create(layout, data, length);
    }

    VertexBuffer::~VertexBuffer() noexcept(false)
    {
        auto api = (VulkanRendererAPI*) RendererAPI::GetInstance();
        auto apiInstance = api->GetData();

        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan:
                delete (VulkanVertexBuffer*) m_Handle;
                break;
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }

    void VertexBuffer::Create(VertexAttributeLayout layout, uint8_t* data, size_t length)
    {
        m_Size = length;
        m_Stride = layout.GetStride();

        auto api = (VulkanRendererAPI*) RendererAPI::GetInstance();
        auto apiInstance = api->GetData();

        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                if (apiInstance.expired()) { return; }
                auto buffer = new VulkanVertexBuffer();
                auto apiInstancePtr = apiInstance.lock();
                buffer->Create(apiInstancePtr.get(), apiInstancePtr->gfxQueue, data, m_Stride * length);
                m_Handle = (VertexBuffer*) buffer;
                break;
            }
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }

    void VertexBuffer::Upload(uint8_t* data, size_t size)
    {
        auto api = (VulkanRendererAPI*) RendererAPI::GetInstance();
        auto apiInstance = api->GetData();

        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                if (apiInstance.expired()) { return; }
                auto apiInstancePtr = apiInstance.lock();
                ((VulkanVertexBuffer*) m_Handle)->Upload(apiInstancePtr.get(), apiInstancePtr->gfxQueue, data, size);
                break;
            }
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }
}// namespace LunaraEngine