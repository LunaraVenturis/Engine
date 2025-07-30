#pragma once
#include <LunaraEngine/Renderer/CommonTypes.hpp>
#include <LunaraEngine/Renderer/Shader.hpp>
#include <LunaraEngine/Renderer/Vulkan/VulkanRendererCommands.hpp>
#include <variant>
#include <expected>
#include <string>

namespace LunaraEngine
{

    class CommandPool;
    class Pipeline;
    struct RendererDataType;
    class VulkanUniformBuffer;
    class VulkanStorageBuffer;
    template <BufferResourceType type>
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
        virtual void* GetBuffer(ShaderBinding binding) override;
        virtual void* GetTexture(ShaderBinding binding) override;

        VkPipeline GetPipeline() const;
        VkPipelineLayout GetPipelineLayout() const;

        const auto& GetDescriptorSets() const { return m_DescriptorSets; }

        auto& GetDescriptorSets() { return m_DescriptorSets; }

        void UpdateBufferDescriptorSets(uint32_t frameIndex);
        void UpdateTextureDescriptorSets(uint32_t frameIndex);

    public:
        static VkFormat GetBufferResourceFormat(BufferResourceFormatT format, BufferResourceDataTypeT type);
        static std::string GetBufferResourceType(BufferResourceType type);
        static std::string GetTextureResourceType(TextureResourceType type);
        static std::string GetBufferResourceLayoutType(BufferResourceMemoryLayout type);
        static size_t GetResourceAttributeSize(BufferResourceAttributeType type);
        auto FindSetLocation(const BufferResourceType type) -> std::expected<size_t, std::string>;

    private:
        void ReadShaderSource(const ShaderInfo& info, std::map<size_t, std::vector<uint32_t>>& shaderSource);
        void LogShaderInfo(const ShaderInfo& info);
        void LogBufferResource(const BufferResource& resource);
        void LogTextureResource(const TextureResource& resource);
        void CreateBuffers(const ShaderInfo& info);
        void CreateTextures();
        void CreateDescriptorSets();
        size_t FindUniformAttributeOffset(std::string_view name);
        VulkanUniformBuffer* GetUniformBuffer(size_t frame);

    private:
        static std::vector<uint32_t> ReadFile(std::filesystem::path name);

    private:
        RendererDataType* m_RendererData{};

        using SetLocation = size_t;
        using BindingLocation = size_t;
        using ResourceLocation = std::pair<SetLocation, BindingLocation>;

        using _BufferResource = Buffer<BufferResourceType::Buffer>*;
        using _TextureResource = Buffer<BufferResourceType::Texture>;
        using BufferResourceList = std::vector<_BufferResource>;   // one for each frame in flight
        using TextureResourceList = std::vector<_TextureResource*>;// one for each frame in flight

        std::map<SetLocation, BufferResourceType> m_SetTypes;
        std::vector<std::map<BindingLocation, std::variant<BufferResourceList, TextureResourceList>>>
                m_Resources;// set -> binding -> resource list for each frame

        std::map<BufferResourceType, std::vector<VkDescriptorSet>> m_DescriptorSets;
        VkDescriptorPool m_DescriptorPool{};
        Pipeline* m_Pipeline{};
        size_t m_UniformBinding{};

        friend class VulkanRendererCommands;
    };
}// namespace LunaraEngine