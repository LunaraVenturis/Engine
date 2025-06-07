#pragma once
#include <Renderer/CommonTypes.hpp>
#include <Renderer/Shader.hpp>

namespace LunaraEngine
{

    class CommandPool;
    class Pipeline;
    struct RendererDataType;
    class VulkanUniformBuffer;

    class VulkanShader: public Shader
    {
    public:
        VulkanShader() = default;
        VulkanShader(RendererDataType* rendererData, const ShaderInfo& info);

        ~VulkanShader();

        void Init(RendererDataType* rendererData, const ShaderInfo& info);
        void Destroy();

    public:
        virtual void SetUniform(std::string_view name, const float& value) override;
        virtual void SetUniform(std::string_view name, const glm::vec2& value) override;
        virtual void SetUniform(std::string_view name, const glm::vec3& value) override;
        virtual void SetUniform(std::string_view name, const glm::vec4& value) override;
        virtual void SetUniform(std::string_view name, const glm::mat3& value) override;
        virtual void SetUniform(std::string_view name, const glm::mat4& value) override;
        virtual void SetUniform(std::string_view name, const int& value) override;
        virtual void SetUniform(std::string_view name, const glm::ivec2& value) override;
        virtual void SetUniform(std::string_view name, const glm::ivec3& value) override;
        virtual void SetUniform(std::string_view name, const glm::ivec4& value) override;

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
        size_t FindUniformAttributeOffset(std::string_view name);

    private:
        static std::vector<uint32_t> ReadFile(std::filesystem::path name);

    private:
        RendererDataType* m_RendererData{};
        std::vector<VulkanUniformBuffer*> m_UniformBuffers{};
        std::vector<VkDescriptorSet> m_DescriptorSets;
        VkDescriptorPool m_DescriptorPool{};
        Pipeline* m_Pipeline{};
    };
}// namespace LunaraEngine