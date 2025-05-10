#pragma once
#include "VulkanDataTypes.hpp"

namespace LunaraEngine
{
    class VulkanInitializer
    {

    public:
        static void Initialize(RendererDataType* rendererData);
        static void Goodbye(RendererDataType* rendererData);

    private:
        VulkanInitializer() = delete;

        VulkanInitializer(RendererDataType* rendererData) : m_RendererData(rendererData) {}

        ~VulkanInitializer() = default;
        VulkanInitializer(const VulkanInitializer& other) = delete;
        VulkanInitializer& operator=(const VulkanInitializer&) = delete;
        VulkanInitializer(VulkanInitializer&& other) = delete;
        VulkanInitializer& operator=(VulkanInitializer&& other) = delete;

    private:
        void CreateInstance();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateSurface();

    private:
        RendererDataType* m_RendererData;
    };

}// namespace LunaraEngine