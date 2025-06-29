#pragma once
#include <Renderer/CommonTypes.hpp>
#include <Renderer/Shader.hpp>
#include <variant>

namespace LunaraEngine
{

    class CommandPool;
    class Pipeline;
    struct RendererDataType;
    class VulkanUniformBuffer;
    class VulkanStorageBuffer;
    class Buffer;

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
        virtual void SetUniform(std::string_view name, const uint32_t& value) override;
        virtual void SetUniform(std::string_view name, const int& value) override;
        virtual void SetUniform(std::string_view name, const glm::ivec2& value) override;
        virtual void SetUniform(std::string_view name, const glm::ivec3& value) override;
        virtual void SetUniform(std::string_view name, const glm::ivec4& value) override;
        virtual void* GetBuffer(size_t binding) override;

        VkPipeline GetPipeline() const;
        VkPipelineLayout GetPipelineLayout() const;

        const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_DescriptorSets; }

        std::vector<VkDescriptorSet>& GetDescriptorSets() { return m_DescriptorSets; }

        void UpdateDescriptorSets(uint32_t frameIndex);

    public:
        static VkFormat GetShaderResourceFormat(ShaderResourceFormatT format, ShaderResourceDataTypeT type);
        static std::string GetShaderResourceType(ShaderResourceType type);
        static std::string GetShaderResourceLayoutType(ShaderResourceMemoryLayout type);
        static size_t GetResourceAttributeSize(ShaderResourceAttributeType type);

    private:
        void ReadShaderSource(const ShaderInfo& info, std::map<size_t, std::vector<uint32_t>>& shaderSource);
        void PrintShaderResource(const ShaderInfo& info);
        void CreateUniformBuffers(const ShaderInfo& info);
        void CreateStorageBuffers(const ShaderInfo& info);
        void CreateDescriptorSets();
        size_t FindUniformAttributeOffset(std::string_view name);
        VulkanUniformBuffer* GetUniformBuffer(size_t frame);

    private:
        static std::vector<uint32_t> ReadFile(std::filesystem::path name);

    private:
        RendererDataType* m_RendererData{};
        std::map<size_t, std::vector<Buffer*>> m_Resources{};
        std::vector<VkDescriptorSet> m_DescriptorSets;
        VkDescriptorPool m_DescriptorPool{};
        Pipeline* m_Pipeline{};
        size_t m_UniformBinding{};
    };
}// namespace LunaraEngine