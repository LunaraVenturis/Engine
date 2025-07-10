#include <Renderer/CommonTypes.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <Renderer/Vulkan/Shader.hpp>
#include <Renderer/Vulkan/Pipeline/PipelineBuilder.hpp>
#include <Renderer/Vulkan/Buffer/UniformBuffer.hpp>
#include <Renderer/Vulkan/Buffer/StorageBuffer.hpp>
#include <Renderer/Vulkan/Buffer/TextureBuffer.hpp>
#include <Renderer/Vulkan/Buffer/Buffer.hpp>
#include <Core/Log.h>
#include "Shader.hpp"

namespace LunaraEngine
{
    VulkanShader::VulkanShader(RendererDataType* rendererData, const ShaderInfo& info) { Init(rendererData, info); }

    void VulkanShader::Init(RendererDataType* rendererData, const ShaderInfo& info)
    {
        p_Info = info;

        PrintBufferResource(info);

        m_RendererData = rendererData;

        std::map<size_t, std::vector<uint32_t>> shaderSource;
        ReadShaderSource(info, shaderSource);

        if (info.isComputeShader) { m_Pipeline = new ComputePipeline(rendererData, &info, shaderSource); }
        else { m_Pipeline = new GraphicsPipeline(rendererData, &info, shaderSource); }

        CreateUniformBuffers(info);
        CreateStorageBuffers(info);
        CreateTextures();
        CreateDescriptorSets();
    }

    void VulkanShader::ReadShaderSource(const ShaderInfo& info, std::map<size_t, std::vector<uint32_t>>& shaderSource)
    {
        if (info.isComputeShader)
        {
            shaderSource[VK_SHADER_STAGE_COMPUTE_BIT] =
                    ReadFile(info.path / std::filesystem::path(info.name + L".comp.spv"));
        }
        else
        {
            shaderSource[VK_SHADER_STAGE_VERTEX_BIT] =
                    ReadFile(info.path / std::filesystem::path(info.name + L".vert.spv"));
            shaderSource[VK_SHADER_STAGE_FRAGMENT_BIT] =
                    ReadFile(info.path / std::filesystem::path(info.name + L".frag.spv"));
        }
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

            for (auto [binding, buffers]: m_BufferResources)
            {
                for (auto buffer: buffers)
                {
                    if (buffer->GetResourceType() == BufferResourceType::UniformBuffer)
                    {
                        delete (VulkanUniformBuffer*) buffer;
                    }
                    else if (buffer->GetResourceType() == BufferResourceType::StorageBuffer)
                    {
                        delete (VulkanStorageBuffer*) buffer;
                    }
                }
            }
            m_BufferResources.clear();

            for (auto [binding, textures]: m_TextureResources)
            {
                for (auto texture: textures) { delete (VulkanTextureBuffer*) texture; }
            }
            m_TextureResources.clear();
            LOG_DEBUG("VulkanShader destroyed");
        }
    }

    size_t VulkanShader::FindUniformAttributeOffset(std::string_view name)
    {
        size_t offset{};
        for (const auto& resource: p_Info.resources.bufferResources)
        {
            if (resource.type == BufferResourceType::UniformBuffer)
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
        return static_cast<VulkanUniformBuffer*>(m_BufferResources[m_UniformBinding][frame]);
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

    void* VulkanShader::GetBuffer(ShaderBinding binding)
    {
        return m_BufferResources[(size_t) binding][m_RendererData->currentFrame];
    }

    void* VulkanShader::GetTexture(ShaderBinding binding)
    {
        return m_TextureResources[(size_t) binding][m_RendererData->currentFrame];
    }

    VkPipeline VulkanShader::GetPipeline() const { return m_Pipeline->GetPipeline(); }

    VkPipelineLayout VulkanShader::GetPipelineLayout() const { return m_Pipeline->GetLayout(); }

    void VulkanShader::CreateUniformBuffers(const ShaderInfo& info)
    {
        for (const auto& resource: info.resources.bufferResources)
        {
            if (resource.type == BufferResourceType::UniformBuffer)
            {
                auto length = resource.length;
                auto stride = resource.stride;
                for (size_t i = 0; i < m_RendererData->maxFramesInFlight; i++)
                {
                    m_BufferResources[(size_t) resource.layout.binding].push_back(
                            new VulkanUniformBuffer(m_RendererData, nullptr, length, stride));
                }
            }
        }
    }

    void VulkanShader::CreateStorageBuffers(const ShaderInfo& info)
    {
        for (const auto& resource: info.resources.bufferResources)
        {
            if (resource.type == BufferResourceType::StorageBuffer)
            {
                auto length = resource.length;
                auto stride = resource.stride;
                for (size_t i = 0; i < m_RendererData->maxFramesInFlight; i++)
                {
                    m_BufferResources[(size_t) resource.layout.binding].push_back(
                            new VulkanStorageBuffer(m_RendererData, nullptr, length, stride));
                }
            }
        }
    }

    void VulkanShader::CreateTextures()
    {
        for (auto& resource: p_Info.resources.textureResources)
        {
            TextureReader reader;
            reader.Open(resource.info.path, resource.info.name);
            resource.info = reader.GetInfo();
            uint8_t* pixelData = reader.Read();
            for (size_t i = 0; i < m_RendererData->maxFramesInFlight; i++)
            {
                m_TextureResources[(size_t) resource.layout.binding].push_back(
                        new VulkanTextureBuffer(m_RendererData, m_RendererData->gfxQueue, reader.GetInfo(), pixelData));
            }
            reader.Close();
        }
    }

    void VulkanShader::CreateDescriptorSets()
    {
        std::vector<VkDescriptorPoolSize> poolSizes;

        for (const auto& resource: p_Info.resources.bufferResources)
        {
            if (resource.type == BufferResourceType::UniformBuffer ||
                resource.type == BufferResourceType::StorageBuffer)
            {
                VkDescriptorPoolSize poolSize{};
                poolSize.type = PipelineBuilder::GetDescriptorType(resource.type);
                poolSize.descriptorCount = m_RendererData->maxFramesInFlight;
                poolSizes.push_back(poolSize);
            }
        }

        for (const auto& resource: p_Info.resources.textureResources)
        {
            VkDescriptorPoolSize poolSize{};
            poolSize.type = PipelineBuilder::GetDescriptorType(resource.type);
            poolSize.descriptorCount = m_RendererData->maxFramesInFlight;
            poolSizes.push_back(poolSize);
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

        std::vector<VkDescriptorSetLayout> layouts(
                m_RendererData->maxFramesInFlight, ((GraphicsPipeline*) m_Pipeline)->GetDescriptorLayout());
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

        const std::optional<std::reference_wrapper<const BufferResource>> bufferResource =
                [&]() -> const std::optional<std::reference_wrapper<const BufferResource>> {
            auto it = std::find_if(p_Info.resources.bufferResources.begin(), p_Info.resources.bufferResources.end(),
                                   [](const BufferResource& resource) {
                                       return resource.type == BufferResourceType::UniformBuffer ||
                                              resource.type == BufferResourceType::StorageBuffer;
                                   });
            return it != p_Info.resources.bufferResources.end()
                           ? std::optional<std::reference_wrapper<const BufferResource>>(*it)
                           : std::nullopt;
        }();

        if (!bufferResource) return;

        for (uint32_t i = 0; i < m_RendererData->maxFramesInFlight; i++) { UpdateDescriptorSets(i); }
    }

    void VulkanShader::UpdateDescriptorSets(uint32_t frameIndex)
    {
        std::vector<VkWriteDescriptorSet> descriptorWrites;
        std::vector<VkDescriptorType> bufferDescriptorTypes;
        std::vector<VkDescriptorType> textureDescriptorTypes;
        std::vector<uint32_t> bufferDescriptorBindings;
        std::vector<uint32_t> textureDescriptorBindings;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> textureInfos;
        for (auto& resource: p_Info.resources.bufferResources)
        {
            if (resource.type == BufferResourceType::PushConstant) { continue; }
            bufferInfos.push_back(VkDescriptorBufferInfo{
                    .buffer = m_BufferResources[(size_t) resource.layout.binding][frameIndex]->GetHandle(),
                    .offset = 0,
                    .range = resource.length * resource.stride,
            });
            bufferDescriptorBindings.push_back((uint32_t) resource.layout.binding);
            bufferDescriptorTypes.push_back(PipelineBuilder::GetDescriptorType(resource.type));
        }
        for (auto& resource: p_Info.resources.textureResources)
        {
            if (resource.type == BufferResourceType::PushConstant) { continue; }

            VulkanTextureBuffer* textureBuffer =
                    static_cast<VulkanTextureBuffer*>(m_TextureResources[(size_t) resource.layout.binding][frameIndex]);

            textureInfos.push_back(VkDescriptorImageInfo{.sampler = textureBuffer->GetSampler(),
                                                         .imageView = textureBuffer->GetView(),
                                                         .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
            textureDescriptorBindings.push_back((uint32_t) resource.layout.binding);
            textureDescriptorTypes.push_back(PipelineBuilder::GetDescriptorType(resource.type));
        }

        for (size_t j = 0; j < bufferDescriptorBindings.size(); j++)
        {
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_DescriptorSets[frameIndex];
            descriptorWrite.dstBinding = bufferDescriptorBindings[j];
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = bufferDescriptorTypes[j];
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfos[j];
            descriptorWrites.push_back(descriptorWrite);
        }
        for (size_t j = 0; j < textureDescriptorBindings.size(); j++)
        {
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_DescriptorSets[frameIndex];
            descriptorWrite.dstBinding = textureDescriptorBindings[j];
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = textureDescriptorTypes[j];
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &textureInfos[j];
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

    void VulkanShader::PrintBufferResource(const ShaderInfo& info)
    {
        LOG_INFO("Shader path: %ls", info.path.wstring().c_str());
        LOG_INFO("Shader name: %ls", info.name.c_str());
        LOG_INFO("Shader resources:");
        LOG_INFO("INPUT RESOURCES");
        for (const auto& resource: info.resources.inputResources)
        {
            LOG_INFO("%s", resource.name.data());
            LOG_INFO("\tType: %s", Shader::GetBufferResourceType(resource.format, resource.type).c_str());
            LOG_INFO("\tBinding: %d", (uint32_t) resource.binding);
            LOG_INFO("\tLocation: %d", resource.location);
            LOG_INFO("\tFormat: %s", Shader::GetBufferResourceFormat(resource.format, resource.type).c_str());
        }
        LOG_INFO("TEXTURE RESOURCES");
        for (const auto& resource: info.resources.textureResources)
        {
            LOG_INFO("%s", resource.name.data());
            LOG_INFO("\tPath: %s", resource.info.path.c_str());
            LOG_INFO("\tName: %ls", resource.info.name.c_str());
            LOG_INFO("\tType: %s", GetBufferResourceType(resource.type).c_str());
            LOG_INFO("\tDimensions: [%u,%u]", resource.info.width, resource.info.height);
            LOG_INFO("\tBit Depth: %u", resource.info.channelDepth);
            LOG_INFO("\tBinding: %d", (uint32_t) resource.layout.binding);
            LOG_INFO("\tLayout: %s", GetBufferResourceLayoutType(resource.layout.layoutType).c_str());
        }
        LOG_INFO("BUFFER RESOURCES");
        for (const auto& resource: info.resources.bufferResources)
        {
            LOG_INFO("%s", resource.name.data());
            LOG_INFO("\tType: %s", GetBufferResourceType(resource.type).c_str());
            LOG_INFO("\tLength: %zu", resource.length);
            LOG_INFO("\tStride: %zu", resource.stride);
            LOG_INFO("\tBinding: %d", (uint32_t) resource.layout.binding);
            LOG_INFO("\tLayout: %s", GetBufferResourceLayoutType(resource.layout.layoutType).c_str());
        }
    }

    std::string VulkanShader::GetBufferResourceType(BufferResourceType type)
    {
        switch (type)
        {
            case BufferResourceType::Texture:
                return "Texture";
            case BufferResourceType::PushConstant:
                return "PushConstant";
            case BufferResourceType::UniformBuffer:
                return "UniformBuffer";
            case BufferResourceType::StorageBuffer:
                return "StorageBuffer";
            default:
                return "Unknown";
        }
    }

    std::string VulkanShader::GetBufferResourceLayoutType(BufferResourceMemoryLayout type)
    {
        switch (type)
        {
            case BufferResourceMemoryLayout::STD140:
                return "STD140";
            case BufferResourceMemoryLayout::STD430:
                return "STD430";
            default:
                return "Unknown";
        }
    }

    size_t VulkanShader::GetResourceAttributeSize(BufferResourceAttributeType type)
    {
        switch (type)
        {
            case BufferResourceAttributeType::Float:
                return sizeof(float);
            case BufferResourceAttributeType::Vec2:
                return sizeof(float) * 2;
            case BufferResourceAttributeType::Vec3:
                return sizeof(float) * 3;
            case BufferResourceAttributeType::Vec4:
                return sizeof(float) * 4;
            case BufferResourceAttributeType::Mat2:
                return sizeof(float) * 4;
            case BufferResourceAttributeType::Mat3:
                return sizeof(float) * 9;
            case BufferResourceAttributeType::Mat4:
                return sizeof(float) * 16;
            case BufferResourceAttributeType::UInt:
                return sizeof(uint32_t);
            case BufferResourceAttributeType::Int:
                return sizeof(int);
            case BufferResourceAttributeType::IVec2:
                return sizeof(int) * 2;
            case BufferResourceAttributeType::IVec3:
                return sizeof(int) * 3;
            case BufferResourceAttributeType::IVec4:
                return sizeof(int) * 4;
            default:
                return 0;
        }
    }

    VkFormat VulkanShader::GetBufferResourceFormat(BufferResourceFormatT format, BufferResourceDataTypeT type)
    {
        switch (format)
        {
            case BufferResourceFormatT::R8:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R8_SRGB;
                else
                    return VK_FORMAT_R8_UNORM;
            case BufferResourceFormatT::R16:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R16_SFLOAT;
                else
                    return VK_FORMAT_R16_UNORM;
            case BufferResourceFormatT::R32:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R32_SFLOAT;
                else
                    return VK_FORMAT_R32_UINT;
            case BufferResourceFormatT::R8G8:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R8G8_SRGB;
                else
                    return VK_FORMAT_R8G8_UNORM;
            case BufferResourceFormatT::R16G16:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R16G16_SFLOAT;
                else
                    return VK_FORMAT_R16G16_UNORM;
            case BufferResourceFormatT::R32G32:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R32G32_SFLOAT;
                else
                    return VK_FORMAT_R32G32_UINT;
            case BufferResourceFormatT::R8G8B8:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R8G8B8_SRGB;
                else
                    return VK_FORMAT_R8G8B8_UNORM;
            case BufferResourceFormatT::R16G16B16:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R16G16B16_SFLOAT;
                else
                    return VK_FORMAT_R16G16B16_UNORM;
            case BufferResourceFormatT::R32G32B32:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R32G32B32_SFLOAT;
                else
                    return VK_FORMAT_R32G32B32_UINT;
            case BufferResourceFormatT::R8G8B8A8:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R8G8B8A8_SRGB;
                else
                    return VK_FORMAT_R8G8B8A8_UNORM;
            case BufferResourceFormatT::R16G16B16A16:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R16G16B16A16_SFLOAT;
                else
                    return VK_FORMAT_R16G16B16A16_UNORM;
            case BufferResourceFormatT::R32G32B32A32:
                if (type == BufferResourceDataTypeT::SFloat) return VK_FORMAT_R32G32B32A32_SFLOAT;
                else
                    return VK_FORMAT_R32G32B32A32_UINT;

            case BufferResourceFormatT::None:
                return VK_FORMAT_UNDEFINED;

            default:
                return VK_FORMAT_UNDEFINED;
        }
    }

}// namespace LunaraEngine
