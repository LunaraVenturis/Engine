#include <LunaraEngine/Renderer/CommonTypes.hpp>
#include <LunaraEngine/Renderer/Shader.hpp>
#include <LunaraEngine/Renderer/Vulkan/VulkanDataTypes.hpp>
#include <LunaraEngine/Renderer/Vulkan/Shader.hpp>
#include <LunaraEngine/Renderer/Vulkan/Pipeline/PipelineBuilder.hpp>
#include <LunaraEngine/Renderer/Vulkan/Buffer/UniformBuffer.hpp>
#include <LunaraEngine/Renderer/Vulkan/Buffer/StorageBuffer.hpp>
#include <LunaraEngine/Renderer/Vulkan/Buffer/TextureBuffer.hpp>
#include <LunaraEngine/Renderer/Vulkan/Buffer/Buffer.hpp>
#include <LunaraEngine/Core/Log.h>
#include "Shader.hpp"
#include <expected>
#include <variant>
#include <future>
#include <execution>
#include <vector>
#include <numeric>
#include <ranges>

namespace LunaraEngine
{
    VulkanShader::VulkanShader(RendererDataType* rendererData, const ShaderInfo& info) { Init(rendererData, info); }

    void VulkanShader::Init(RendererDataType* rendererData, const ShaderInfo& info)
    {
        p_Info = info;

        LogShaderInfo(info);

        m_RendererData = rendererData;

        std::map<size_t, std::vector<uint32_t>> shaderSource;
        ReadShaderSource(info, shaderSource);

        if (info.isComputeShader) { m_Pipeline = new ComputePipeline(rendererData, &info, shaderSource); }
        else { m_Pipeline = new GraphicsPipeline(rendererData, &info, shaderSource); }

        CreateBuffers(info);
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
            for (auto& resource: m_Resources)
            {
                for (auto& [binding, buffers]: resource)
                {
                    if (std::holds_alternative<BufferResourceList>(buffers))
                    {
                        for (auto buffer: std::get<BufferResourceList>(buffers))
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
                    else
                    {
                        for (auto& texture: std::get<TextureResourceList>(buffers))
                        {
                            delete (VulkanTextureBuffer*) texture;
                        }
                    }
                }
                resource.clear();
            }
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
        if (auto result = FindSetLocation(BufferResourceType::Buffer); result.has_value())
        {
            return static_cast<VulkanUniformBuffer*>(
                    std::get<BufferResourceList>(m_Resources[*result][m_UniformBinding])[frame]);
        }
        return nullptr;
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

    auto VulkanShader::FindSetLocation(const BufferResourceType type) -> std::expected<size_t, std::string>
    {
        auto result = [&]() -> std::expected<size_t, std::string> {
            auto it = std::ranges::find_if(m_SetTypes, [type = type](const auto& pair) { return pair.second == type; });
            return it != m_SetTypes.end() ? std::expected<size_t, std::string>{std::in_place_t{}, (*it).first}
                                          : std::unexpected("Could not find buffer set location!");
        }();
        return result;
    }

    void* VulkanShader::GetBuffer(ShaderBinding binding)
    {
        if (auto result = FindSetLocation(BufferResourceType::Buffer); result.has_value())
        {
            return std::get<BufferResourceList>(
                    m_Resources[*result][std::to_underlying(binding)])[m_RendererData->currentFrame];
        }
        return nullptr;
    }

    void* VulkanShader::GetTexture(ShaderBinding binding)
    {
        if (auto result = FindSetLocation(BufferResourceType::Texture); result.has_value())
        {
            return std::get<BufferResourceList>(
                    m_Resources[*result][std::to_underlying(binding)])[m_RendererData->currentFrame];
        }
        return nullptr;
    }

    VkPipeline VulkanShader::GetPipeline() const { return m_Pipeline->GetPipeline(); }

    VkPipelineLayout VulkanShader::GetPipelineLayout() const { return m_Pipeline->GetLayout(); }

    void VulkanShader::CreateBuffers(const ShaderInfo& info)
    {
        for (const auto& resource: info.resources.bufferResources)
        {
            if (resource.type == BufferResourceType::UniformBuffer ||
                resource.type == BufferResourceType::StorageBuffer)
            {
                // Add set if it doesn't exist
                if (std::ranges::find_if(m_SetTypes, [set = resource.layout.set](const auto& type) {
                        return type.first == set;
                    }) == m_SetTypes.end())
                {
                    m_SetTypes[resource.layout.set] = BufferResourceType::Buffer;
                }
                if (resource.layout.set >= m_Resources.size()) { m_Resources.resize(resource.layout.set + 1); }

                //Create buffer list for binding
                auto& bufferList = std::get<BufferResourceList>(
                        m_Resources[resource.layout.set][std::to_underlying(resource.layout.binding)]);

                // Resize buffer list to have buffer for each frame
                bufferList.resize(m_RendererData->maxFramesInFlight);

                // Crate buffer for each frame
                std::ranges::for_each(bufferList, [&](auto& buffer) {
                    switch (resource.type)
                    {
                        case BufferResourceType::UniformBuffer:
                            buffer = new VulkanUniformBuffer(m_RendererData, nullptr, resource.length, resource.stride);
                            break;
                        case BufferResourceType::StorageBuffer:
                            buffer = new VulkanStorageBuffer(m_RendererData, nullptr, resource.length, resource.stride);
                            break;
                        default:
                            break;
                    }
                });
            }
        }
    }

    void VulkanShader::CreateTextures()
    {
        for (auto& resource: p_Info.resources.textureResources)
        {
            // Add set if it doesn't exist
            if (std::ranges::find_if(m_SetTypes, [set = resource.layout.set](const auto& type) {
                    return type.first == set;
                }) == m_SetTypes.end())
            {
                m_SetTypes[resource.layout.set] = BufferResourceType::Texture;
            }
            if (resource.layout.set >= m_Resources.size())
            {
                std::map<BindingLocation, std::variant<BufferResourceList, TextureResourceList>> temp;
                temp[std::to_underlying(resource.layout.binding)] = TextureResourceList{};
                m_Resources.push_back(std::move(temp));
            }

            //Create buffer list for binding
            auto& textureList = std::get<TextureResourceList>(
                    m_Resources[resource.layout.set][std::to_underlying(resource.layout.binding)]);

            // Resize buffer list to have buffer for each frame
            textureList.resize(m_RendererData->maxFramesInFlight);

            // Allocate texture buffer handle for each frame
            std::ranges::for_each(textureList, [&](auto& buffer) {
                switch (resource.textureType)
                {
                    case TextureResourceType::Texture2D:
                    case TextureResourceType::Texture2DArray:
                    case TextureResourceType::Texture2DBindlessArray:
                        buffer = new VulkanTextureBuffer();
                        break;
                    default:
                        break;
                }
            });

            std::vector<std::expected<TextureDataView, std::error_code>> readTextureDataResults;
            readTextureDataResults.resize(resource.textureNames.size());

            // Read textures asynchronously
            auto indices = std::views::iota(size_t{0}, resource.textureNames.size());
            std::for_each(std::execution::par, indices.begin(), indices.end(), [&](size_t i) {
                readTextureDataResults[i] = TextureReader::Read(
                        std::filesystem::path(resource.path), std::wstring(resource.textureNames[i]));
                LOG_INFO("Loaded texture: %ls", resource.textureNames[i].data());
            });

            // Extract pixel data from results
            auto values = readTextureDataResults | std::views::filter([](const auto& result) {
                              if (!result.has_value()) { LOG_ERROR("%s", result.error().message().data()); }
                              return result.has_value();
                          }) |
                          std::views::transform([](auto& e) { return *e; });
            std::vector<TextureDataView> texturesPixelData(values.begin(), values.end());

            // Copy texture data to texture buffer objects
            std::ranges::for_each(textureList, [&](_TextureResource* texture) {
                VulkanTextureBuffer* vulkanTexture = static_cast<VulkanTextureBuffer*>(texture);
                switch (resource.textureType)
                {
                    case TextureResourceType::Texture2D: {
                        TextureInfo textureInfo = TextureReader::GetInfo(resource.path, resource.textureNames[0]);
                        vulkanTexture->Create(
                                m_RendererData, m_RendererData->gfxQueue, textureInfo, &texturesPixelData[0]);
                        break;
                    }
                    case TextureResourceType::Texture2DArray: {
                        // Get texture width and height (all textures have same size)
                        TextureInfo textureInfo = TextureReader::GetInfo(resource.path, resource.textureNames[0]);
                        resource.width = textureInfo.width;
                        resource.height = textureInfo.height;
                        vulkanTexture->Create(m_RendererData, m_RendererData->gfxQueue, resource, texturesPixelData);
                    }
                    default:
                        break;
                }
            });
        }
    }

    void VulkanShader::CreateDescriptorSets()
    {
        std::map<VkDescriptorType, uint32_t> descriptorCountMap;

        for (const auto& resource: p_Info.resources.bufferResources)
        {
            if (resource.type == BufferResourceType::UniformBuffer ||
                resource.type == BufferResourceType::StorageBuffer)
            {
                auto type = PipelineBuilder::GetDescriptorType(resource.type);
                descriptorCountMap[type] += m_RendererData->maxFramesInFlight;
            }
        }

        for (const auto& resource: p_Info.resources.textureResources)
        {
            auto type = PipelineBuilder::GetDescriptorType(resource.resourceType);
            descriptorCountMap[type] += m_RendererData->maxFramesInFlight;
        }

        std::vector<VkDescriptorPoolSize> poolSizes;
        for (const auto& [type, count]: descriptorCountMap)
        {
            VkDescriptorPoolSize poolSize{};
            poolSize.type = type;
            poolSize.descriptorCount = count;
            poolSizes.push_back(poolSize);
        }

        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        // maximum descriptors in pool
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        // maximum number of descriptor sets
        if (p_Info.resources.textureResources.size() > 0)
        {
            // 2x number of frames in flight ( one for textures and one for buffers)
            descriptorPoolInfo.maxSets = 2 * m_RendererData->maxFramesInFlight;
        }
        else
        {
            // 1x number of frames in flight ( one for buffers)
            descriptorPoolInfo.maxSets = 1 * m_RendererData->maxFramesInFlight;
        }
        descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        if (vkCreateDescriptorPool(m_RendererData->device, &descriptorPoolInfo, nullptr, &m_DescriptorPool) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        auto getLayouts = [&](const size_t setLocation, size_t countPerType) {
            auto result = std::ranges::find_if(m_Pipeline->GetDescriptorLayouts(),
                                               [setLocation](const auto& pair) { return pair.first == setLocation; });
            if (result != m_Pipeline->GetDescriptorLayouts().end())
            {
                return std::vector(countPerType, result->second);
            }
            return std::vector<VkDescriptorSetLayout>(countPerType, (VkDescriptorSetLayout) VK_NULL_HANDLE);
        };

        auto allocateSets = [&](const auto& layouts, const size_t countPerType) {
            VkDescriptorSetAllocateInfo allocateInfo{};
            allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocateInfo.descriptorPool = m_DescriptorPool;
            allocateInfo.descriptorSetCount = static_cast<uint32_t>(countPerType);
            allocateInfo.pSetLayouts = layouts.data();

            std::vector<VkDescriptorSet> sets(countPerType);
            if (vkAllocateDescriptorSets(m_RendererData->device, &allocateInfo, sets.data()) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }
            return sets;
        };

        for (const auto& [location, type]: m_SetTypes)
        {
            auto layouts = getLayouts(location, m_RendererData->maxFramesInFlight);
            auto sets = allocateSets(layouts, m_RendererData->maxFramesInFlight);
            m_DescriptorSets[type] = sets;
        }

        const std::optional<std::reference_wrapper<const BufferResource>> bufferResource =
                [&]() -> const std::optional<std::reference_wrapper<const BufferResource>> {
            auto it = std::ranges::find_if(p_Info.resources.bufferResources, [](const BufferResource& resource) {
                return resource.type == BufferResourceType::UniformBuffer ||
                       resource.type == BufferResourceType::StorageBuffer;
            });
            return it != p_Info.resources.bufferResources.end()
                           ? std::optional<std::reference_wrapper<const BufferResource>>(*it)
                           : std::nullopt;
        }();

        if (!bufferResource) return;

        for (uint32_t i = 0; i < m_RendererData->maxFramesInFlight; i++)
        {
            UpdateBufferDescriptorSets(i);
            UpdateTextureDescriptorSets(i);
        }
    }

    void VulkanShader::UpdateTextureDescriptorSets(uint32_t frameIndex)
    {
        std::vector<VkWriteDescriptorSet> descriptorWrites;
        std::vector<VkDescriptorType> textureDescriptorTypes;
        std::vector<uint32_t> textureDescriptorBindings;
        std::vector<VkDescriptorImageInfo> textureInfos;

        auto logError = [](const std::string& message) {
            LOG_ERROR("Failed to update texture descriptor sets");
            LOG_ERROR("%s", message.c_str());
            return false;
        };

        auto updateTextureSets = [&](size_t setIndex) -> std::expected<bool, std::string> {
            std::ranges::for_each(p_Info.resources.textureResources, [&](const TextureResource& resource) {
                VulkanTextureBuffer* textureBuffer = static_cast<VulkanTextureBuffer*>(std::get<TextureResourceList>(
                        m_Resources[setIndex][(size_t) resource.layout.binding])[frameIndex]);

                textureInfos.push_back(VkDescriptorImageInfo{.sampler = textureBuffer->GetSampler(),
                                                             .imageView = textureBuffer->GetView(),
                                                             .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
                textureDescriptorBindings.push_back((uint32_t) resource.layout.binding);
                textureDescriptorTypes.push_back(PipelineBuilder::GetDescriptorType(resource.resourceType));
            });

            for (size_t j = 0; j < textureDescriptorBindings.size(); j++)
            {
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = m_DescriptorSets[BufferResourceType::Texture][frameIndex];
                descriptorWrite.dstBinding = textureDescriptorBindings[j];
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = textureDescriptorTypes[j];
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pImageInfo = &textureInfos[j];
                descriptorWrites.push_back(descriptorWrite);
            }
            if (descriptorWrites.size() == 0) return std::unexpected("Descriptor write count is 0!");
            vkUpdateDescriptorSets(m_RendererData->device, static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(), 0, nullptr);
            return true;
        };

        FindSetLocation(BufferResourceType::Texture).and_then(updateTextureSets).transform_error(logError);
    }

    void VulkanShader::UpdateBufferDescriptorSets(uint32_t frameIndex)
    {
        std::vector<VkWriteDescriptorSet> descriptorWrites;
        std::vector<VkDescriptorType> bufferDescriptorTypes;
        std::vector<uint32_t> bufferDescriptorBindings;
        std::vector<VkDescriptorBufferInfo> bufferInfos;

        auto logError = [](const std::string& message) {
            LOG_ERROR("Failed to update buffer descriptor sets");
            LOG_ERROR("%s", message.c_str());
            return false;
        };

        auto updateBufferSets = [&](size_t setIndex) -> std::expected<bool, std::string> {
            std::ranges::for_each(p_Info.resources.bufferResources, [&](const BufferResource& resource) {
                if (resource.type == BufferResourceType::PushConstant) { return; }

                bufferInfos.push_back(VkDescriptorBufferInfo{
                        .buffer = std::get<BufferResourceList>(
                                          m_Resources[setIndex][(size_t) resource.layout.binding])[frameIndex]
                                          ->GetHandle(),
                        .offset = 0,
                        .range = resource.length * resource.stride,
                });
                bufferDescriptorBindings.push_back((uint32_t) resource.layout.binding);
                bufferDescriptorTypes.push_back(PipelineBuilder::GetDescriptorType(resource.type));
            });

            for (size_t j = 0; j < bufferDescriptorBindings.size(); j++)
            {
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = m_DescriptorSets[BufferResourceType::Buffer][frameIndex];
                descriptorWrite.dstBinding = bufferDescriptorBindings[j];
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = bufferDescriptorTypes[j];
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = &bufferInfos[j];
                descriptorWrites.push_back(descriptorWrite);
            }
            if (descriptorWrites.size() == 0) return std::unexpected("Descriptor write count is 0!");
            vkUpdateDescriptorSets(m_RendererData->device, static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(), 0, nullptr);
            return true;
        };

        FindSetLocation(BufferResourceType::Buffer).and_then(updateBufferSets).transform_error(logError);
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

    void VulkanShader::LogShaderInfo(const ShaderInfo& info)
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
        for (const auto& resource: info.resources.textureResources) { LogTextureResource(resource); }
        LOG_INFO("BUFFER RESOURCES");
        for (const auto& resource: info.resources.bufferResources) { LogBufferResource(resource); }
    }

    void VulkanShader::LogBufferResource(const BufferResource& resource)
    {
        LOG_INFO("Resource name %s", resource.name.data());
        LOG_INFO("\tType: %s", GetBufferResourceType(resource.type).c_str());
        LOG_INFO("\tLength: %zu", resource.length);
        LOG_INFO("\tStride: %zu", resource.stride);
        LOG_INFO("\tSet: %d", (uint32_t) resource.layout.set);
        LOG_INFO("\tBinding: %d", (uint32_t) resource.layout.binding);
        LOG_INFO("\tLayout: %s", GetBufferResourceLayoutType(resource.layout.layoutType).c_str());
    }

    void VulkanShader::LogTextureResource(const TextureResource& resource)
    {
        LOG_INFO("Resource name %s", resource.name.data());
        LOG_INFO("\tPath: %s", resource.path.c_str());
        LOG_INFO("\tResource Type: %s", GetBufferResourceType(resource.resourceType).c_str());
        LOG_INFO("\tTexture Type: %s", GetTextureResourceType(resource.textureType).c_str());
        LOG_INFO("\tDimensions: [%u,%u]", resource.width, resource.height);
        LOG_INFO("\tBit Depth: %u", resource.channelDepth);
        LOG_INFO("\tSet: %d", (uint32_t) resource.layout.set);
        LOG_INFO("\tBinding: %d", (uint32_t) resource.layout.binding);
        LOG_INFO("\tLayout: %s", GetBufferResourceLayoutType(resource.layout.layoutType).c_str());

        if (resource.textureType != TextureResourceType::Texture2D)
        {
            LOG_INFO("\tTextures:");
            for (const auto& name: resource.textureNames) { LOG_INFO("\t\tName: %ls", name.data()); }
        }
        else { LOG_INFO("\tTexture Name: %ls", resource.textureNames[0].data()); }
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

    std::string VulkanShader::GetTextureResourceType(TextureResourceType type)
    {
        switch (type)
        {
            case TextureResourceType::Texture2D:
                return "Texture2D";
            case TextureResourceType::Texture2DArray:
                return "Texture2DArray";
            case TextureResourceType::Texture2DBindlessArray:
                return "Texture2DBindlessArray";
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
