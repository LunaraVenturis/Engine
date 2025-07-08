#pragma once
#include "StorageBuffer.hpp"
#include <Renderer/Vulkan/Buffer/StorageBuffer.hpp>
#include <Renderer/Vulkan/VulkanRendererAPI.hpp>

namespace LunaraEngine
{
    using std::size_t;

    template <typename T>
    StorageBuffer<T>::StorageBuffer(size_t length, size_t stride)
    {
        Create<T>(nullptr, length, stride);
    }

    template <typename T>
    StorageBuffer<T>::StorageBuffer(T* data, size_t length, size_t stride)
    {
        Create<T>(data, length, stride);
    }

    template <typename T>
    StorageBuffer<T>::~StorageBuffer()
    {
        delete (VulkanStorageBuffer*) m_Handle;
    }

    template <typename T>
    void StorageBuffer<T>::Upload(T* data, size_t length, size_t stride)
    {
        auto api = (VulkanRendererAPI*) RendererAPI::GetInstance();
        auto apiInstance = api->GetData();

        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                if (apiInstance.expired()) { return; }
                auto apiInstancePtr = apiInstance.lock();
                ((VulkanStorageBuffer*) m_Handle)
                        ->Upload(apiInstancePtr.get(), apiInstancePtr->gfxQueue, data, length, stride);
                break;
            }
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }

    template <typename T>
    void StorageBuffer<T>::Create(T* data, size_t length, size_t stride)
    {
        m_Size = stride * length;
        m_Stride = stride;

        auto api = (VulkanRendererAPI*) RendererAPI::GetInstance();
        auto apiInstance = api->GetData();

        switch (RendererAPI::GetAPIType())
        {
            case RendererAPIType::Vulkan: {
                if (apiInstance.expired()) { return; }
                auto buffer = new VulkanStorageBuffer();
                auto apiInstancePtr = apiInstance.lock();
                buffer->Create(apiInstancePtr.get(), (uint8_t*) data, length, stride);
                m_Handle = (StorageBuffer*) buffer;
                break;
            }
            default:
                throw std::runtime_error("Unknown renderer API");
                break;
        }
    }

}// namespace LunaraEngine