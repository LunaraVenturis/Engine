#include <Renderer/CommonTypes.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <Renderer/Vulkan/Shader.hpp>
#include <Renderer/Vulkan/GraphicsPipeline.hpp>
#include <Renderer/Vulkan/UniformBuffer.hpp>
#include <Core/Log.h>
#include "Shader.hpp"

namespace LunaraEngine
{

    void VulkanShader::Create(RendererDataType* rendererData, const ShaderInfo& info)
    {
        m_Info = info;
        if (info.isComputeShader) { throw std::runtime_error("Compute shaders are not supported"); }

        PrintShaderResource(info);

        m_RendererData = rendererData;

        std::map<size_t, std::vector<uint32_t>> shaderSource;
        ReadShaderSource(info, shaderSource);

        m_Pipeline = new GraphicsPipeline(rendererData, &info, shaderSource);

        CreateUniformBuffers(info);
        CreateDescriptorSets();
    }

    void VulkanShader::ReadShaderSource(const ShaderInfo& info, std::map<size_t, std::vector<uint32_t>>& shaderSource)
    {
        shaderSource[VK_SHADER_STAGE_VERTEX_BIT] =
                ReadFile(info.path / std::filesystem::path(info.name + L".vert.spv"));
        shaderSource[VK_SHADER_STAGE_FRAGMENT_BIT] =
                ReadFile(info.path / std::filesystem::path(info.name + L".frag.spv"));
    }

    VulkanShader::~VulkanShader() { Destroy(); }

    void VulkanShader::Destroy()
    {
        if (m_Pipeline != nullptr)
        {
            vkDeviceWaitIdle(m_RendererData->device);
            delete (GraphicsPipeline*) m_Pipeline;
            m_Pipeline = nullptr;

            for (auto uniformBuffer: m_UniformBuffers) { delete uniformBuffer; }
            m_UniformBuffers.clear();
        }
    }

    void VulkanShader::SetUniform(std::string_view name, const glm::vec3& value)
    {
        size_t offset{};
        for (const auto& resource: m_Info.resources.bufferResources)
        {
            if (resource.type == ShaderResourceType::UniformBuffer)
            {
                for (const auto& attribute: resource.attributes)
                {
                    if (attribute.name == name) { break; }
                    offset += VulkanShader::GetResourceAttributeSize(attribute.type);
                }
            }
        }
        m_UniformBuffers[m_RendererData->currentFrame]->Upload(m_RendererData, offset, (uint8_t*) &value,
                                                               sizeof(glm::vec3));
    }

    VkPipeline VulkanShader::GetPipeline() const { return m_Pipeline->GetPipeline(); }

    VkPipelineLayout VulkanShader::GetPipelineLayout() const { return m_Pipeline->GetLayout(); }

    void VulkanShader::CreateUniformBuffers(const ShaderInfo& info)
    {
        for (const auto& resource: info.resources.bufferResources)
        {
            if (resource.type == ShaderResourceType::UniformBuffer)
            {
                auto size = resource.size;
                for (size_t i = 0; i < m_RendererData->maxFramesInFlight; i++)
                {
                    m_UniformBuffers.push_back(new VulkanUniformBuffer(m_RendererData, nullptr, 1, size));
                }
            }
        }
    }

    void VulkanShader::CreateDescriptorSets()
    {
        VkDescriptorPoolSize descriptorPoolSize{};
        descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSize.descriptorCount = m_RendererData->maxFramesInFlight;

        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = 1;
        descriptorPoolInfo.pPoolSizes = &descriptorPoolSize;
        descriptorPoolInfo.maxSets = m_RendererData->maxFramesInFlight;
        descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        if (vkCreateDescriptorPool(m_RendererData->device, &descriptorPoolInfo, nullptr, &m_DescriptorPool) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        std::vector<VkDescriptorSetLayout> layouts(m_RendererData->maxFramesInFlight,
                                                   ((GraphicsPipeline*) m_Pipeline)->GetDescriptorLayout());
        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = m_DescriptorPool;
        allocateInfo.descriptorSetCount = m_RendererData->maxFramesInFlight;
        allocateInfo.pSetLayouts = layouts.data();

        m_DescriptorSets.resize(m_RendererData->maxFramesInFlight);

        if (vkAllocateDescriptorSets(m_RendererData->device, &allocateInfo, m_DescriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        const std::optional<std::reference_wrapper<const ShaderResource>> bufferResource =
                [&]() -> const std::optional<std::reference_wrapper<const ShaderResource>> {
            auto it = std::find_if(
                    m_Info.resources.bufferResources.begin(), m_Info.resources.bufferResources.end(),
                    [](const ShaderResource& resource) { return resource.type == ShaderResourceType::UniformBuffer; });
            return it != m_Info.resources.bufferResources.end()
                           ? std::optional<std::reference_wrapper<const ShaderResource>>(*it)
                           : std::nullopt;
        }();

        if (!bufferResource) return;

        for (size_t i = 0; i < m_RendererData->maxFramesInFlight; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_UniformBuffers[i]->GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = bufferResource->get().size;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_DescriptorSets[i];
            descriptorWrite.dstBinding = bufferResource->get().layout.binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            vkUpdateDescriptorSets(m_RendererData->device, 1, &descriptorWrite, 0, nullptr);
        }
    }

    std::vector<uint32_t> VulkanShader::ReadFile(std::filesystem::path name)
    {

        std::ifstream file(name, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            std::string path = name.string();
            LOG_ERROR("Failed to open file: %s", path.c_str());
            throw std::runtime_error("failed to open file!");
        }

        std::streamsize fileSize = file.tellg();
        if (fileSize < 0) { throw std::runtime_error("failed to determine file size!"); }
        std::vector<uint32_t> buffer(static_cast<size_t>(fileSize));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();

        return buffer;
    }

    void VulkanShader::PrintShaderResource(const ShaderInfo& info)
    {
        LOG_INFO("Shader path: %ls", info.path.wstring().c_str());
        LOG_INFO("Shader name: %ls", info.name.c_str());
        LOG_INFO("Shader resources:");
        LOG_INFO("INPUT RESOURCES");
        for (const auto& resource: info.resources.inputResources)
        {
            LOG_INFO("%s", resource.name.data());
            LOG_INFO("\tType: %s", Shader::GetShaderResourceType(resource.format, resource.type).c_str());
            LOG_INFO("\tBinding: %d", resource.binding);
            LOG_INFO("\tLocation: %d", resource.location);
            LOG_INFO("\tFormat: %s", Shader::GetShaderResourceFormat(resource.format, resource.type).c_str());
        }
        LOG_INFO("BUFFER RESOURCES");

        for (const auto& resource: info.resources.bufferResources)
        {
            LOG_INFO("%s", resource.name.data());
            LOG_INFO("\tType: %s", GetShaderResourceType(resource.type).c_str());
            LOG_INFO("\tSize: %zu", resource.size);
            LOG_INFO("\tBinding: %d", resource.layout.binding);
            LOG_INFO("\tLayout: %s", GetShaderResourceLayoutType(resource.layout.layoutType).c_str());
        }
    }

    std::string VulkanShader::GetShaderResourceType(ShaderResourceType type)
    {
        switch (type)
        {
            case ShaderResourceType::Texture:
                return "Texture";
            case ShaderResourceType::Sampler:
                return "Sampler";
            case ShaderResourceType::PushConstant:
                return "PushConstant";
            case ShaderResourceType::UniformBuffer:
                return "UniformBuffer";
            case ShaderResourceType::StorageBuffer:
                return "StorageBuffer";
            default:
                return "Unknown";
        }
    }

    std::string VulkanShader::GetShaderResourceLayoutType(ShaderResourceMemoryLayout type)
    {
        switch (type)
        {
            case ShaderResourceMemoryLayout::STD140:
                return "STD140";
            case ShaderResourceMemoryLayout::STD430:
                return "STD430";
            default:
                return "Unknown";
        }
    }

    size_t VulkanShader::GetResourceAttributeSize(ShaderResourceAttributeType type)
    {
        switch (type)
        {
            case ShaderResourceAttributeType::Float:
                return sizeof(float);
            case ShaderResourceAttributeType::Vec2:
                return sizeof(float) * 2;
            case ShaderResourceAttributeType::Vec3:
                return sizeof(float) * 3;
            case ShaderResourceAttributeType::Vec4:
                return sizeof(float) * 4;
            case ShaderResourceAttributeType::Mat2:
                return sizeof(float) * 4;
            case ShaderResourceAttributeType::Mat3:
                return sizeof(float) * 9;
            case ShaderResourceAttributeType::Mat4:
                return sizeof(float) * 16;
            case ShaderResourceAttributeType::Int:
                return sizeof(int);
            case ShaderResourceAttributeType::IVec2:
                return sizeof(int) * 2;
            case ShaderResourceAttributeType::IVec3:
                return sizeof(int) * 3;
            case ShaderResourceAttributeType::IVec4:
                return sizeof(int) * 4;
            default:
                return 0;
        }
    }

    VkFormat VulkanShader::GetShaderResourceFormat(ShaderResourceFormatT format, ShaderResourceDataTypeT type)
    {
        switch (format)
        {
            case ShaderResourceFormatT::R8:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R8_SRGB;
                else
                    return VK_FORMAT_R8_UNORM;
            case ShaderResourceFormatT::R16:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R16_SFLOAT;
                else
                    return VK_FORMAT_R16_UNORM;
            case ShaderResourceFormatT::R32:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R32_SFLOAT;
                else
                    return VK_FORMAT_R32_UINT;
            case ShaderResourceFormatT::R8G8:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R8G8_SRGB;
                else
                    return VK_FORMAT_R8G8_UNORM;
            case ShaderResourceFormatT::R16G16:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R16G16_SFLOAT;
                else
                    return VK_FORMAT_R16G16_UNORM;
            case ShaderResourceFormatT::R32G32:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R32G32_SFLOAT;
                else
                    return VK_FORMAT_R32G32_UINT;
            case ShaderResourceFormatT::R8G8B8:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R8G8B8_SRGB;
                else
                    return VK_FORMAT_R8G8B8_UNORM;
            case ShaderResourceFormatT::R16G16B16:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R16G16B16_SFLOAT;
                else
                    return VK_FORMAT_R16G16B16_UNORM;
            case ShaderResourceFormatT::R32G32B32:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R32G32B32_SFLOAT;
                else
                    return VK_FORMAT_R32G32B32_UINT;
            case ShaderResourceFormatT::R8G8B8A8:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R8G8B8A8_SRGB;
                else
                    return VK_FORMAT_R8G8B8A8_UNORM;
            case ShaderResourceFormatT::R16G16B16A16:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R16G16B16A16_SFLOAT;
                else
                    return VK_FORMAT_R16G16B16A16_UNORM;
            case ShaderResourceFormatT::R32G32B32A32:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R32G32B32A32_SFLOAT;
                else
                    return VK_FORMAT_R32G32B32A32_UINT;

            case ShaderResourceFormatT::None:
                return VK_FORMAT_UNDEFINED;

            default:
                return VK_FORMAT_UNDEFINED;
        }
    }

    FlatInstancedShader::FlatInstancedShader(std::filesystem::path assetsDirectory) { Init(assetsDirectory); }

    void FlatInstancedShader::Init(std::filesystem::path assetsDirectory)
    {
        ShaderResources basicShaderResources;
        basicShaderResources.bufferResources.push_back(ShaderResource{
                .type = ShaderResourceType::UniformBuffer,
                .name = "UniformBuffer",
                .size = sizeof(glm::vec3),
                .layout = ShaderResourceLayout{.binding = 0, .layoutType = ShaderResourceMemoryLayout::STD140},
                .attributes = {
                        ShaderResourceAttribute{.name = "offset", .type = ShaderResourceAttributeType::Vec3},
                }});

        ShaderInfo basicShaderInfo;
        basicShaderInfo.isComputeShader = false;
        basicShaderInfo.name = L"FlatInstanced";
        basicShaderInfo.path = assetsDirectory / "Shaders/output";
        basicShaderInfo.resources = basicShaderResources;

        basicShaderInfo.resources.inputResources.push_back(ShaderInputResource{.name = "Position",
                                                                               .binding = 0,
                                                                               .location = 0,
                                                                               .format = ShaderResourceFormatT::R32G32,
                                                                               .type = ShaderResourceDataTypeT::SFloat,
                                                                               .offset = 0});
        basicShaderInfo.resources.inputResources.push_back(
                ShaderInputResource{.name = "Color",
                                    .binding = 0,
                                    .location = 1,
                                    .format = ShaderResourceFormatT::R32G32B32,
                                    .type = ShaderResourceDataTypeT::SFloat,
                                    .offset = 8});
        Shader::Init(basicShaderInfo);
    }
}// namespace LunaraEngine
