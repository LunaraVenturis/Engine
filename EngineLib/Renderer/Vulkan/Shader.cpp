#include <Renderer/CommonTypes.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <Renderer/Vulkan/Shader.hpp>
#include <Renderer/Vulkan/GraphicsPipeline.hpp>
#include <Renderer/Vulkan/UniformBuffer.hpp>
#include <Renderer/Vulkan/StorageBuffer.hpp>
#include <Renderer/Vulkan/Buffer.hpp>
#include <Core/Log.h>
#include "Shader.hpp"

namespace LunaraEngine
{
    VulkanShader::VulkanShader(RendererDataType* rendererData, const ShaderInfo& info) { Init(rendererData, info); }

    void VulkanShader::Init(RendererDataType* rendererData, const ShaderInfo& info)
    {
        p_Info = info;
        if (info.isComputeShader) { throw std::runtime_error("Compute shaders are not supported"); }

        PrintShaderResource(info);

        m_RendererData = rendererData;

        std::map<size_t, std::vector<uint32_t>> shaderSource;
        ReadShaderSource(info, shaderSource);

        m_Pipeline = new GraphicsPipeline(rendererData, &info, shaderSource);

        CreateUniformBuffers(info);
        CreateStorageBuffers(info);
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
            vkDestroyDescriptorPool(m_RendererData->device, m_DescriptorPool, nullptr);
            delete (GraphicsPipeline*) m_Pipeline;
            m_Pipeline = nullptr;

            for (auto [binding, buffers]: m_Resources)
            {
                for (auto buffer: buffers)
                {
                    if (buffer->GetResourceType() == ShaderResourceType::UniformBuffer)
                    {
                        delete (VulkanUniformBuffer*) buffer;
                    }
                    else if (buffer->GetResourceType() == ShaderResourceType::StorageBuffer)
                    {
                        delete (VulkanStorageBuffer*) buffer;
                    }
                }
            }
            m_Resources.clear();
            LOG_DEBUG("VulkanShader destroyed");
        }
    }

    size_t VulkanShader::FindUniformAttributeOffset(std::string_view name)
    {
        size_t offset{};
        for (const auto& resource: p_Info.resources.bufferResources)
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
        return offset;
    }

    VulkanUniformBuffer* VulkanShader::GetUniformBuffer(size_t frame)
    {
        return static_cast<VulkanUniformBuffer*>(m_Resources[m_UniformBinding][frame]);
    }

    void VulkanShader::SetUniform(std::string_view name, const float& value)
    {
        size_t offset = FindUniformAttributeOffset(name);
        GetUniformBuffer(m_RendererData->currentFrame)->Upload(offset, (uint8_t*) &value, 1, sizeof(float));
    }

    void VulkanShader::SetUniform(std::string_view name, const glm::vec2& value)
    {
        size_t offset = FindUniformAttributeOffset(name);
        GetUniformBuffer(m_RendererData->currentFrame)->Upload(offset, (uint8_t*) &value, 1, sizeof(glm::vec2));
    }

    void VulkanShader::SetUniform(std::string_view name, const glm::vec3& value)
    {
        size_t offset = FindUniformAttributeOffset(name);
        GetUniformBuffer(m_RendererData->currentFrame)->Upload(offset, (uint8_t*) &value, 1, sizeof(glm::vec3));
    }

    void VulkanShader::SetUniform(std::string_view name, const glm::vec4& value)
    {
        size_t offset = FindUniformAttributeOffset(name);
        GetUniformBuffer(m_RendererData->currentFrame)->Upload(offset, (uint8_t*) &value, 1, sizeof(glm::vec4));
    }

    void VulkanShader::SetUniform(std::string_view name, const glm::mat3& value)
    {
        size_t offset = FindUniformAttributeOffset(name);
        GetUniformBuffer(m_RendererData->currentFrame)->Upload(offset, (uint8_t*) &value, 1, sizeof(glm::mat3));
    }

    void VulkanShader::SetUniform(std::string_view name, const glm::mat4& value)
    {
        size_t offset = FindUniformAttributeOffset(name);
        GetUniformBuffer(m_RendererData->currentFrame)->Upload(offset, (uint8_t*) &value, 1, sizeof(glm::mat4));
    }

    void VulkanShader::SetUniform(std::string_view name, const uint32_t& value)
    {
        size_t offset = FindUniformAttributeOffset(name);
        GetUniformBuffer(m_RendererData->currentFrame)->Upload(offset, (uint8_t*) &value, 1, sizeof(uint32_t));
    }

    void VulkanShader::SetUniform(std::string_view name, const int& value)
    {
        size_t offset = FindUniformAttributeOffset(name);
        GetUniformBuffer(m_RendererData->currentFrame)->Upload(offset, (uint8_t*) &value, 1, sizeof(int));
    }

    void VulkanShader::SetUniform(std::string_view name, const glm::ivec2& value)
    {
        size_t offset = FindUniformAttributeOffset(name);
        GetUniformBuffer(m_RendererData->currentFrame)->Upload(offset, (uint8_t*) &value, 1, sizeof(glm::ivec2));
    }

    void VulkanShader::SetUniform(std::string_view name, const glm::ivec3& value)
    {
        size_t offset = FindUniformAttributeOffset(name);
        GetUniformBuffer(m_RendererData->currentFrame)->Upload(offset, (uint8_t*) &value, 1, sizeof(glm::ivec3));
    }

    void VulkanShader::SetUniform(std::string_view name, const glm::ivec4& value)
    {
        size_t offset = FindUniformAttributeOffset(name);
        GetUniformBuffer(m_RendererData->currentFrame)->Upload(offset, (uint8_t*) &value, 1, sizeof(glm::ivec4));
    }

    void* VulkanShader::GetBuffer(size_t binding) { return m_Resources[binding][m_RendererData->currentFrame]; }

    VkPipeline VulkanShader::GetPipeline() const { return m_Pipeline->GetPipeline(); }

    VkPipelineLayout VulkanShader::GetPipelineLayout() const { return m_Pipeline->GetLayout(); }

    void VulkanShader::CreateUniformBuffers(const ShaderInfo& info)
    {
        for (const auto& resource: info.resources.bufferResources)
        {
            if (resource.type == ShaderResourceType::UniformBuffer)
            {
                auto length = resource.length;
                auto stride = resource.stride;
                for (size_t i = 0; i < m_RendererData->maxFramesInFlight; i++)
                {
                    m_Resources[resource.layout.binding].push_back(
                            new VulkanUniformBuffer(m_RendererData, nullptr, length, stride));
                }
            }
        }
    }

    void VulkanShader::CreateStorageBuffers(const ShaderInfo& info)
    {
        for (const auto& resource: info.resources.bufferResources)
        {
            if (resource.type == ShaderResourceType::StorageBuffer)
            {
                auto length = resource.length;
                auto stride = resource.stride;
                for (size_t i = 0; i < m_RendererData->maxFramesInFlight; i++)
                {
                    m_Resources[resource.layout.binding].push_back(
                            new VulkanStorageBuffer(m_RendererData, nullptr, length, stride));
                }
            }
        }
    }

    void VulkanShader::CreateDescriptorSets()
    {
        std::vector<VkDescriptorPoolSize> poolSizes;

        for (const auto& resource: p_Info.resources.bufferResources)
        {
            if (resource.type == ShaderResourceType::UniformBuffer ||
                resource.type == ShaderResourceType::StorageBuffer)
            {
                VkDescriptorPoolSize poolSize{};
                poolSize.type = GraphicsPipeline::GetDescriptorType(resource.type);
                poolSize.descriptorCount = m_RendererData->maxFramesInFlight;
                poolSizes.push_back(poolSize);
            }
        }

        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
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
            auto it = std::find_if(p_Info.resources.bufferResources.begin(), p_Info.resources.bufferResources.end(),
                                   [](const ShaderResource& resource) {
                                       return resource.type == ShaderResourceType::UniformBuffer ||
                                              resource.type == ShaderResourceType::StorageBuffer;
                                   });
            return it != p_Info.resources.bufferResources.end()
                           ? std::optional<std::reference_wrapper<const ShaderResource>>(*it)
                           : std::nullopt;
        }();

        if (!bufferResource) return;

        for (uint32_t i = 0; i < m_RendererData->maxFramesInFlight; i++) { UpdateDescriptorSets(i); }
    }

    void VulkanShader::UpdateDescriptorSets(uint32_t frameIndex)
    {

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        std::vector<VkDescriptorType> descriptorTypes;
        std::vector<uint32_t> descriptorBindings;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        for (auto& resource: p_Info.resources.bufferResources)
        {
            if (resource.type == ShaderResourceType::PushConstant) { continue; }
            bufferInfos.push_back(VkDescriptorBufferInfo{
                    .buffer = m_Resources[resource.layout.binding][frameIndex]->GetBuffer(),
                    .offset = 0,
                    .range = resource.length * resource.stride,
            });
            descriptorBindings.push_back(resource.layout.binding);
            descriptorTypes.push_back(GraphicsPipeline::GetDescriptorType(resource.type));
        }

        for (size_t j = 0; j < descriptorBindings.size(); j++)
        {
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_DescriptorSets[frameIndex];
            descriptorWrite.dstBinding = descriptorBindings[j];
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = descriptorTypes[j];
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfos[j];
            descriptorWrites.push_back(descriptorWrite);
        }
        vkUpdateDescriptorSets(m_RendererData->device, static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
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
            LOG_INFO("\tLength: %zu", resource.length);
            LOG_INFO("\tStride: %zu", resource.stride);
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
            case ShaderResourceAttributeType::UInt:
                return sizeof(uint32_t);
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

}// namespace LunaraEngine
