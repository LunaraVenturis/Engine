#pragma once
#include "IndexBuffer.hpp"
#include <Renderer/Vulkan/IndexBuffer.hpp>
#include <Renderer/Vulkan/VulkanRendererAPI.hpp>

namespace LunaraEngine
{
    using std::size_t;

    template <typename T>
    IndexBuffer<T>::IndexBuffer(size_t length, size_t stride)
    {
        Create<T>(nullptr, length, stride);
    }

    template <typename T>
    IndexBuffer<T>::IndexBuffer(T* data, size_t length, size_t stride)
    {
        Create<T>(data, length, stride);
    }

    template <typename T>
    IndexBuffer<T>::~IndexBuffer()
    {
        delete (VulkanIndexBuffer*) m_Handle;
    }

    template <typename T>
    void IndexBuffer<T>::Upload(T* data, size_t length, size_t stride)
    {
        auto api = (VulkanRendererAPI*) RendererAPI::GetInstance();
        auto apiInstance = api->GetData();

        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                if (apiInstance.expired()) { return; }
                auto apiInstancePtr = apiInstance.lock();
                ((VulkanIndexBuffer*) m_Handle)
                        ->Upload(apiInstancePtr.get(), apiInstancePtr->gfxQueue, data, length, stride);
                break;
            }
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }

    template <typename T>
    void IndexBuffer<T>::Create(T* data, size_t length, size_t stride)
    {
        m_Size = stride * length;
        m_Stride = stride;

        auto api = (VulkanRendererAPI*) RendererAPI::GetInstance();
        auto apiInstance = api->GetData();

        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                if (apiInstance.expired()) { return; }
                auto buffer = new VulkanIndexBuffer();
                auto apiInstancePtr = apiInstance.lock();
                buffer->Create(apiInstancePtr.get(), apiInstancePtr->gfxQueue, (uint8_t*) data, length, stride);
                m_Handle = (IndexBuffer*) buffer;
                break;
            }
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }

}// namespace LunaraEngine