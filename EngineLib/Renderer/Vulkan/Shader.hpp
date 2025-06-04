#pragma once
#include <Renderer/CommonTypes.hpp>

namespace LunaraEngine
{

    class CommandPool;
    class Pipeline;
    struct RendererDataType;
    class VulkanUniformBuffer;

    class VulkanShader
    {
    public:
        VulkanShader() = default;
        ~VulkanShader();

    public:
        void Create(RendererDataType* rendererData, const ShaderInfo& info);
        void Destroy();
        VkPipeline GetPipeline() const;

    public:
        static VkFormat GetShaderResourceFormat(ShaderResourceFormatT format, ShaderResourceDataTypeT type);
        static std::string GetShaderResourceType(ShaderResourceType type);
        static std::string GetShaderResourceLayoutType(ShaderResourceMemoryLayout type);

    private:
        void ReadShaderSource(const ShaderInfo& info, std::map<size_t, std::vector<uint32_t>>& shaderSource);
        void PrintShaderResource(const ShaderInfo& info);
        void CreateUniformBuffers(const ShaderInfo& info);

    private:
        static std::vector<uint32_t> ReadFile(std::filesystem::path name);

    private:
        RendererDataType* m_RendererData{};
        std::vector<VulkanUniformBuffer*> m_UniformBuffers{};
        Pipeline* m_Pipeline{};
    };
}// namespace LunaraEngine