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

        void Create(RendererDataType* rendererData, const ShaderInfo& info);
        void Destroy();

    public:
        void SetUniform(std::string_view name, const glm::vec3& value);
        VkPipeline GetPipeline() const;
        VkPipelineLayout GetPipelineLayout() const;

        const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_DescriptorSets; }

    public:
        static VkFormat GetShaderResourceFormat(ShaderResourceFormatT format, ShaderResourceDataTypeT type);
        static std::string GetShaderResourceType(ShaderResourceType type);
        static std::string GetShaderResourceLayoutType(ShaderResourceMemoryLayout type);
        static size_t GetResourceAttributeSize(ShaderResourceAttributeType type);

    private:
        void ReadShaderSource(const ShaderInfo& info, std::map<size_t, std::vector<uint32_t>>& shaderSource);
        void PrintShaderResource(const ShaderInfo& info);
        void CreateUniformBuffers(const ShaderInfo& info);
        void CreateDescriptorSets();

    private:
        static std::vector<uint32_t> ReadFile(std::filesystem::path name);

    private:
        ShaderInfo m_Info;
        RendererDataType* m_RendererData{};
        std::vector<VulkanUniformBuffer*> m_UniformBuffers{};
        std::vector<VkDescriptorSet> m_DescriptorSets;
        VkDescriptorPool m_DescriptorPool{};
        Pipeline* m_Pipeline{};
    };
}// namespace LunaraEngine